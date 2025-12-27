#pragma once
#include "Entity.h"
#include "SFML/System/Vector3.hpp"
#include "Vertex.h"
#include <SFML/Graphics.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class World {
private:
  std::unordered_map<std::string, sf::Texture> texture_map;
  std::unordered_map<std::string, std::vector<Vertex>> model_map;

public:
  luabridge::LuaRef onStart;
  luabridge::LuaRef onUpdate;
  luabridge::LuaRef onRender;

  World(lua_State *L, std::string script);
  std::vector<std::unique_ptr<Vertex>> vertices;
  std::vector<std::unique_ptr<Entity>> entities;

  double friction;
  double gravity;
  std::string sky_texture;
  std::string ground_texture;

  sf::Vector3f camera;

  sf::Texture *load_texture(std::string texture);
  const std::vector<Vertex> &load_model(std::string model);

  void add_vertex(Vertex *vertex);
  void add_entity(Entity *entity);
  void vertex_from_model(std::string model);

  void spawn_model(std::string model, sf::Vector3f position);
  void spawn_entity(std::string entity, sf::Vector3f position);

  void interact(Entity &entity, double distance);
  void destroyEntity(Entity &entity);

  void cleanup();

  void clear_cache();

  static void initLua(lua_State *L);
};
