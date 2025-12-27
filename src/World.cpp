#include "World.h"
#include "Entity.h"
#include "LuaBridge/detail/LuaException.h"
#include "Physics.h"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <mutex>
#include <rapidcsv.h>
#include <spdlog/spdlog.h>
#include <string>

std::mutex texture_mutex;
std::mutex model_mutex;
std::mutex entity_mutex;

World::World(lua_State *L, std::string script)
    : onStart(L), onUpdate(L), onRender(L) {

  if (luaL_dofile(L, script.c_str()) == LUA_OK) {
    luabridge::LuaRef scriptTable = luabridge::LuaRef::fromStack(L, -1);

    onStart = scriptTable["on_start"];
    onUpdate = scriptTable["on_update"];
    onRender = scriptTable["on_render"];
    lua_pop(L, 1);
  } else {
    spdlog::error("Failed to read script {}", script);
  }
}

sf::Texture *World::load_texture(std::string texture) {
  std::lock_guard<std::mutex> lock(texture_mutex);
  if (!texture_map.contains(texture)) {
    texture_map[texture] = sf::Texture();
    if (texture.empty()) {
      std::filesystem::path resources("resources");
      std::string placeholder = resources / "sprites" / "placeholder.png";
      texture_map[texture].loadFromFile(placeholder);
    } else {
      texture_map[texture].loadFromFile(texture);
    }
  }
  return &texture_map[texture];
}

const std::vector<Vertex> &World::load_model(std::string model) {
  std::lock_guard<std::mutex> lock(model_mutex);
  if (model.empty()) {
    if (!model_map.contains("")) {
      model_map[""] = std::vector<Vertex>();
    }
    return model_map[""];
  }
  if (!model_map.contains(model)) {
    rapidcsv::Document document(model);
    std::vector<Vertex> vertices;
    vertices.reserve(document.GetRowCount());
    int x1Idx = document.GetColumnIdx("x1");
    int y1Idx = document.GetColumnIdx("y1");
    int x2Idx = document.GetColumnIdx("x2");
    int y2Idx = document.GetColumnIdx("y2");
    int heightIdx = document.GetColumnIdx("height");
    int zIdx = document.GetColumnIdx("z");
    int modelIdx = document.GetColumnIdx("model");

    if (x1Idx == -1 || y1Idx == -1 || x2Idx == -1 || y2Idx == -1) {
      spdlog::error("Failed to find required columns in model {}", model);
      model_map[model] = std::vector<Vertex>();
      return model_map[model];
    }

    spdlog::info("Loading model {}: {} vertices", model,
                 document.GetRowCount());

    for (int i = 0; i < document.GetRowCount(); ++i) {
      vertices.emplace_back(Vertex({document.GetCell<float>(x1Idx, i),
                                    document.GetCell<float>(y1Idx, i)},
                                   {document.GetCell<float>(x2Idx, i),
                                    document.GetCell<float>(y2Idx, i)},
                                   document.GetCell<float>(heightIdx, i),
                                   document.GetCell<float>(zIdx, i),
                                   document.GetCell<std::string>(modelIdx, i)));
    }
    model_map[model] = std::move(vertices);
  }
  return model_map[model];
}

void World::add_vertex(Vertex *vertex) {
  vertices.push_back(std::unique_ptr<Vertex>(vertex));
}

void World::add_entity(Entity *entity) {
  entities.push_back(std::unique_ptr<Entity>(entity));
}

void World::vertex_from_model(std::string model) {
  const std::vector<Vertex> &vertecies = load_model(model);
  for (const Vertex &vertex : vertecies) {
    add_vertex(new Vertex(vertex));
  }
}

void World::spawn_model(std::string model, sf::Vector3f position) {
  const std::vector<Vertex> &vertices = load_model(model);
  for (const Vertex &vertex : vertices) {
    Vertex temp = vertex.translated(position);
    this->vertices.push_back(std::make_unique<Vertex>(temp));
  }
}

void World::spawn_entity(std::string entity, sf::Vector3f position) {
  if (std::filesystem::exists(entity)) {
    lua_State *L = onStart.state();
    Entity *entityObject = new Entity(L, entity, position);
    try {
      entityObject->onStart(entityObject);
    } catch (const luabridge::LuaException &e) {
      spdlog::error("Luabridge execption starting entity {}", e.what());
    }
    add_entity(entityObject);
    spdlog::info("Spawning entity {}", entity);
  } else {
    spdlog::error("Could not locate entity {}", entity);
  }
}

void World::interact(Entity &entity, double distance) {
  std::vector<CastResult> results =
      Physics::cast_ray(*this, entity.location, entity.location.z);
  for (CastResult &result : results) {
    if (result.owner == &entity) {
      continue;
    }
    if (result.distance <= distance) {
      if (result.owner) {
        result.owner->onInteract(result.owner, &entity);
      }
      break;
    }
  }
}

void World::destroyEntity(Entity &entity) {
  entity.deleted = true;
  entity.onDeath(&entity);
}

void World::update(double dt) {
  try {
    if (onUpdate) {
      onUpdate(this, dt);
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption updating world {}", e.what());
  }

  for (int i = 0; i < entities.size(); ++i) {
    Entity *entity = entities[i].get();
    if (entity and not entity->deleted) {
      entity->update(dt);
    }
  }
}

void World::clear_cache() {
  texture_map.clear();
  model_map.clear();
}

void World::cleanup() {
  for (int i = 0; i < entities.size(); ++i) {
    if (entities[i]->deleted) {
      entities.erase(entities.begin() + i);
    }
  }
}

void World::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<World>("World")
      .addProperty("friction", &World::friction)
      .addProperty("gravity", &World::gravity)
      .addProperty("sky_texture", &World::sky_texture)
      .addProperty("ground_texture", &World::ground_texture)
      .addProperty("camera", &World::camera)
      .addFunction("load_texture", &World::load_texture)
      .addFunction("vertex_from_model", &World::vertex_from_model)
      .addFunction("spawn_model", &World::spawn_model)
      .addFunction("spawn_entity", &World::spawn_entity)
      .addFunction("interact", &World::interact)
      .addFunction("destroy_entity", &World::destroyEntity)
      .endClass();
}
