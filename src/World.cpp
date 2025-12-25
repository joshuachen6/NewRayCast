#include "World.h"
#include "Entity.h"
#include "LuaBridge/detail/LuaException.h"
#include <boost/algorithm/string.hpp>
#include <filesystem>
#include <fstream>
#include <mutex>
#include <spdlog/spdlog.h>
#include <string>

std::mutex texture_mutex;
std::mutex model_mutex;
std::mutex entity_mutex;

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

sf::Texture *World::load_texture(const std::string &texture) {
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

const std::vector<Vertex> &World::load_model(const std::string &model) {
  std::lock_guard<std::mutex> lock(model_mutex);
  if (!model_map.contains(model)) {
    std::ifstream file(model);
    if (file.is_open()) {
      model_map[model] = std::vector<Vertex>();
      std::string line;

      std::vector<std::string> data;
      std::vector<std::string> start(2), end(2);

      while (std::getline(file, line)) {
        boost::trim(line);
        boost::erase_all(line, " ");

        boost::split(data, line, boost::is_any_of(";"));

        boost::split(start, data[0], boost::is_any_of(","));
        boost::split(end, data[1], boost::is_any_of(","));

        Vertex vertex(sf::Vector2f(std::stod(start[0]), std::stod(start[1])),
                      sf::Vector2f(std::stod(end[0]), std::stod(end[1])),
                      std::stod(data[2]), std::stod(data[3]), data[4]);
        model_map[model].push_back(vertex);
      }
    } else {
      spdlog::error("Failed to load model {}", model);
    }
    file.close();
  }
  return model_map[model];
}

void World::add_vertex(Vertex *vertex) {
  vertices.push_back(std::unique_ptr<Vertex>(vertex));
}

void World::add_entity(Entity *entity) {
  entities.push_back(std::unique_ptr<Entity>(entity));
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

void World::clear_cache() {
  texture_map.clear();
  model_map.clear();
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
      .addFunction("spawn_model", &World::spawn_model)
      .addFunction("spawn_entity", &World::spawn_entity)
      .endClass();
}
