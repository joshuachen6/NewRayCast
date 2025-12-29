#include "Game.h"
#include "LuaBridge/detail/Namespace.h"
#include "Renderer.h"
#include <memory>
#include <spdlog/spdlog.h>

Game::Game(lua_State *L, std::string script)
    : onUpdate(L), onStart(L), onWorldSwitch(L), onRender(L) {

  if (luaL_dofile(L, script.c_str()) == LUA_OK) {
    luabridge::LuaRef scriptTable = luabridge::LuaRef::fromStack(L, -1);

    onStart = scriptTable["on_start"];
    onUpdate = scriptTable["on_update"];
    onWorldSwitch = scriptTable["on_world_switch"];
    onRender = scriptTable["on_render"];

    lua_pop(L, 1);
  } else {
    spdlog::error("Failed to read script {}", script);
  }
}

void Game::load_world(std::string world) {
  spdlog::info("Loading world {}", world);
  worlds[world] = std::make_unique<World>(onStart.state(), world);
  try {
    if (worlds[world]->onStart) {
      worlds[world]->onStart(worlds[world].get());
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption starting world {}", e.what());
  }
}

void Game::switch_world(std::string world) {
  spdlog::info("Switching world to {}", world);
  if (not world.empty()) {
    if (!worlds.contains(world)) {
      load_world(world);
    }
    currentWorld = world;
    try {
      if (onWorldSwitch) {
        onWorldSwitch(this, world);
      }
    } catch (const luabridge::LuaException &e) {
      spdlog::error("Lua error on world switch: {}", e.what());
    }
  }
}

void Game::reset_world(std::string world) { load_world(world); }

World *Game::getWorld() {
  if (currentWorld.empty()) {
    return nullptr;
  }
  return worlds[currentWorld].get();
}

void Game::update(double dt) {
  try {
    if (onUpdate) {
      onUpdate(this, dt);
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption updating game {}", e.what());
  }
  if (getWorld()) {
    getWorld()->update(dt);
  }
}

void Game::render(Renderer &renderer) {
  try {
    if (onRender) {
      onRender(this, &renderer);
    }
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption on render {}", e.what());
  }
}

void Game::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<Game>("Game")
      .addFunction("get_world", &Game::getWorld)
      .addFunction("switch_world", &Game::switch_world)
      .addFunction("reset_world", &Game::reset_world)

      .endClass();
}
