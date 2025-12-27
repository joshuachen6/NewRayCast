#pragma once
#include "World.h"

#include <lua.h>

#include <LuaBridge/LuaBridge.h>
#include <memory>
#include <unordered_map>

class Game {
private:
  std::unordered_map<std::string, std::unique_ptr<World>> worlds;

  std::string currentWorld;

  void load_world(std::string world);

public:
  luabridge::LuaRef onUpdate;
  luabridge::LuaRef onStart;
  luabridge::LuaRef onWorldSwitch;

  Game(lua_State *L, std::string script);

  void switch_world(std::string world);
  void reset_world(std::string world);
  World *getWorld();
  void update(double dt);

  static void initLua(lua_State *L);
};
