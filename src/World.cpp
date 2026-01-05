#include "Entity.h"
#include "LuaBridge/detail/LuaException.h"
#include "Physics.h"
#include "World.h"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <mutex>
#include <rapidcsv.h>
#include <spdlog/spdlog.h>
#include <string>

std::mutex texture_mutex;
std::mutex model_mutex;
std::mutex entity_mutex;
std::mutex scene_mutex;

World::World(lua_State *L, std::string script) : onStart(L), onUpdate(L) {

  if (luaL_dofile(L, script.c_str()) == LUA_OK) {
    luabridge::LuaRef scriptTable = luabridge::LuaRef::fromStack(L, -1);

    onStart = scriptTable["on_start"];
    onUpdate = scriptTable["on_update"];
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
    if (not model_map.contains("")) {
      model_map[""] = std::vector<Vertex>();
    }
    return model_map[""];
  }
  if (not model_map.contains(model)) {
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

    if (x1Idx == -1 or y1Idx == -1 or x2Idx == -1 or y2Idx == -1) {
      spdlog::error("Failed to find required columns in model {}", model);
      model_map[model] = std::vector<Vertex>();
      return model_map[model];
    }

    spdlog::info("Loading model {}: {} vertices", model, document.GetRowCount());

    for (int i = 0; i < document.GetRowCount(); ++i) {
      vertices.emplace_back(Vertex({document.GetCell<float>(x1Idx, i), document.GetCell<float>(y1Idx, i)},
                                   {document.GetCell<float>(x2Idx, i), document.GetCell<float>(y2Idx, i)}, document.GetCell<float>(heightIdx, i),
                                   document.GetCell<float>(zIdx, i), document.GetCell<std::string>(modelIdx, i)));
    }
    model_map[model] = std::move(vertices);
  }
  return model_map[model];
}

void World::add_vertex(Vertex *vertex) {
  vertices.push_back(std::unique_ptr<Vertex>(vertex));

  // Add to relevant cells
  int xmin = std::floor(std::min(vertex->start.x, vertex->end.x) / cellSize);
  int xmax = std::floor(std::max(vertex->start.x, vertex->end.x) / cellSize);
  int ymin = std::floor(std::min(vertex->start.y, vertex->end.y) / cellSize);
  int ymax = std::floor(std::max(vertex->start.y, vertex->end.y) / cellSize);

  for (int y = ymin; y <= ymax; ++y) {
    for (int x = xmin; x <= xmax; ++x) {
      cells[getKey(x, y)].vertices.push_back(vertex);
    }
  }
}

void World::add_entity(Entity *entity) { entities.push_back(std::unique_ptr<Entity>(entity)); }

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
    add_vertex(new Vertex(temp));
  }
}

Entity *World::spawn_entity(std::string entity, sf::Vector3f position) {
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
    return entityObject;
  }

  spdlog::error("Could not locate entity {}", entity);
  return nullptr;
}

void World::spawn_scene(std::string scene, sf::Vector3f position) {
  std::lock_guard<std::mutex> lock(scene_mutex);
  if (scene.empty()) {
    return;
  }
  rapidcsv::Document document(scene);
  int xIdx = document.GetColumnIdx("x");
  int yIdx = document.GetColumnIdx("y");
  int zIdx = document.GetColumnIdx("z");
  int entityIdx = document.GetColumnIdx("entity");

  if (xIdx == -1 or yIdx == -1 or entityIdx == -1) {
    spdlog::error("Failed to find required columns for scene {}", scene);
    return;
  }

  spdlog::info("Loading scene {}: {} entities", scene, document.GetRowCount());

  for (int i = 0; i < document.GetRowCount(); ++i) {
    float x = document.GetCell<float>(xIdx, i);
    float y = document.GetCell<float>(yIdx, i);
    float z = document.GetCell<float>(zIdx, i);

    std::string entity = document.GetCell<std::string>(entityIdx, i);

    x = x * cos(position.z) - y * sin(position.z);
    y = x * sin(position.z) + y * cos(position.z);
    z += position.z;

    spawn_entity(entity, {x, y, z});
  }
}

void World::interact(Entity &entity, double distance) {
  std::vector<CastResult> results = Physics::cast_ray(*this, entity.location, entity.location.z, distance);
  for (CastResult &result : results) {
    if (result.owner == &entity) {
      continue;
    }
    if (result.owner) {
      if (result.owner->onInteract) {
        result.owner->onInteract(result.owner, &entity);
      }
    }
    break;
  }
}

void World::destroyEntity(Entity &entity) {
  entity.onDeath(&entity);
  for (uint64_t key : entity.cells) {
    std::vector<Entity *> &entities = cells[key].entities;
    auto it = std::find(entities.begin(), entities.end(), &entity);
    if (it != entities.end()) {
      *it = entities.back();
      entities.pop_back();
    }
  }
  entity.deleted = true;
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
      if (entity->model_changed) {
        entity->vertecies = load_model(entity->model);
        entity->model_changed = false;
      }
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
      --i;
    }
  }
}

uint64_t World::getKey(int x, int y) { return ((uint64_t)(uint32_t)x << 32) | (uint32_t)y; }

void World::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<World>("World")
      .addProperty("friction", &World::friction)
      .addProperty("gravity", &World::gravity)
      .addProperty("brightness", &World::brightness)
      .addProperty("sky_texture", &World::sky_texture)
      .addProperty("ground_texture", &World::ground_texture)
      .addProperty("camera", &World::camera)
      .addFunction("load_texture", &World::load_texture)
      .addFunction("vertex_from_model", &World::vertex_from_model)
      .addFunction("spawn_model", &World::spawn_model)
      .addFunction("spawn_entity", &World::spawn_entity)
      .addFunction("spawn_scene", &World::spawn_scene)
      .addFunction("interact", &World::interact)
      .addFunction("destroy_entity", &World::destroyEntity)
      .endClass();
}
