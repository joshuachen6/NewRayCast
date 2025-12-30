#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_set>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class Entity {
public:
  std::string model;
  std::string script;
  std::string id;
  std::unordered_set<std::string> tags;
  sf::Vector3f location;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  double mass = 1.0;
  double radius = 1.0;
  double health = 100.0;
  bool is_static = false;
  bool has_collision = true;
  bool deleted = false;

  luabridge::LuaRef onStart;
  luabridge::LuaRef onCollide;
  luabridge::LuaRef onUpdate;
  luabridge::LuaRef onInteract;
  luabridge::LuaRef onDamage;
  luabridge::LuaRef onDeath;

  Entity(lua_State *L, const std::string &script,
         sf::Vector3f location = sf::Vector3f());

  void addTag(std::string tag);
  bool hasTag(std::string tag);
  void damage(float damage);
  void update(double dt);

  static void initLua(lua_State *L);
};
