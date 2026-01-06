#include "Entity.h"
#include <boost/algorithm/string.hpp>
#include <lauxlib.h>
#include <spdlog/spdlog.h>

Entity::Entity(lua_State *L, const std::string &script, sf::Vector3f location)
    : onStart(L), onCollide(L), onUpdate(L), onInteract(L), onDamage(L), onDeath(L) {
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

  translate();
}

void Entity::addTag(std::string tag) { tags.insert(tag); }

bool Entity::hasTag(std::string tag) { return this->tags.contains(tag); }

void Entity::setModel(std::string model) {
  this->model = model;
  model_changed = true;
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

void Entity::translate() {
  translated.clear();
  translated.reserve(vertecies.size());
  for (Vertex &vertex : vertecies) {
    translated.push_back(std::move(vertex.translated(location)));
  }
  lastLocation = location;
}

void Entity::update(double dt) {
  if (lastLocation != location) {
    translate();
  }
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
      .addProperty("script", &Entity::script)
      .addProperty("id", &Entity::id)
      .addProperty("location", &Entity::location)
      .addProperty("velocity", &Entity::velocity)
      .addProperty("acceleration", &Entity::acceleration)
      .addProperty("mass", &Entity::mass)
      .addProperty("radius", &Entity::radius)
      .addProperty("health", &Entity::health)
      .addProperty("deleted", &Entity::deleted)
      .addProperty("is_static", &Entity::is_static)
      .addProperty("has_collision", &Entity::has_collision)
      .addFunction("set_model", &Entity::setModel)
      .addFunction("damage", &Entity::damage)
      .addFunction("add_tag", &Entity::addTag)
      .addFunction("has_tag", &Entity::hasTag)
      .endClass();
}
