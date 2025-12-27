#include "Entity.h"
#include <boost/algorithm/string.hpp>
#include <lauxlib.h>
#include <spdlog/spdlog.h>

Entity::Entity(lua_State *L, const std::string &script, sf::Vector3f location)
    : onStart(L), onCollide(L), onUpdate(L), onInteract(L), onDamage(L),
      onDeath(L) {
  this->location = location;
  this->script = script;

  if (luaL_dofile(L, script.c_str()) == LUA_OK) {
    luabridge::LuaRef scriptTable = luabridge::LuaRef::fromStack(L, -1);

    onStart = scriptTable["on_start"];
    onUpdate = scriptTable["on_update"];
    onCollide = scriptTable["on_collide"];
    onDamage = scriptTable["on_damage"];
    onInteract = scriptTable["on_interact"];
    onDeath = scriptTable["on_death"];
    lua_pop(L, 1);
  } else {
    spdlog::error("Failed to read script {}", script);
  }
}

void Entity::damage(float damage) {
  try {
    if (onDamage) {
      onDamage(this, damage);
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption damaging entity {}", e.what());
  }
}

void Entity::update(double dt) {
  try {
    if (onUpdate) {
      onUpdate(this, dt);
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption updating entity {}", e.what());
  }
}

void Entity::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<Entity>("Entity")
      .addProperty("model", &Entity::model)
      .addProperty("script", &Entity::script)
      .addProperty("location", &Entity::location)
      .addProperty("velocity", &Entity::velocity)
      .addProperty("acceleration", &Entity::acceleration)
      .addProperty("mass", &Entity::mass)
      .addProperty("radius", &Entity::radius)
      .addProperty("health", &Entity::health)
      .addProperty("is_static", &Entity::is_static)
      .addFunction("damage", &Entity::damage)
      .endClass();
}
