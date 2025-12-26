#pragma once
#include <SFML/Graphics.hpp>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class Entity {
public:
  std::string model;
  std::string script;
  sf::Vector3f location;
  sf::Vector2f velocity;
  sf::Vector2f acceleration;
  double mass = 1.0;
  double radius = 1.0;
  bool is_static = false;

  luabridge::LuaRef onStart;
  luabridge::LuaRef onCollide;
  luabridge::LuaRef onUpdate;
  luabridge::LuaRef onInteract;
  luabridge::LuaRef onDamage;

  Entity(lua_State *L, const std::string &script,
         sf::Vector3f location = sf::Vector3f());

  static void initLua(lua_State *L);
};
