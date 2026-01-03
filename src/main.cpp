#include "Audio.h"
#include "Game.h"
#include "Renderer.h"
#include "World.h"
#include <chrono>
#include <lua.h>
#define _USE_MATH_DEFINES
#include "Physics.h"
#include <Controls.h>
#include <filesystem>
#include <math.h>
#include <spdlog/spdlog.h>

int main() {

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  // get the folder for the resources
  std::filesystem::path resourceFolder("resources");

  // is the game in focus
  bool focus = true;

  sf::RenderWindow window(sf::VideoMode(1280, 720), "Game");
  sf::View view(sf::FloatRect(0, 0, 1280, 720));
  window.setView(view);

  Renderer renderer(window);

  Controls::initLua(L);
  Audio::initLua(L);
  Physics::initLua(L);
  Entity::initLua(L);
  World::initLua(L);
  Game::initLua(L);
  Renderer::initLua(L);

  Game game(L, resourceFolder / "scripts" / "game.lua");
  Controls controls;
  Audio audio;

  luabridge::setGlobal(L, &game, "game");
  luabridge::setGlobal(L, &audio, "audio");
  luabridge::setGlobal(L, &controls, "controls");

  try {
    if (game.onStart)
      game.onStart(&game);
  } catch (const luabridge::LuaException &e) {
    spdlog::error("Luabridge execption starting game {}", e.what());
  }

  std::chrono::time_point<std::chrono::system_clock> last =
      std::chrono::system_clock::now();

  while (window.isOpen() and game.running) {

    double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    std::chrono::system_clock::now() - last)
                    .count() /
                1e9;
    last = std::chrono::system_clock::now();

    sf::Event event;
    while (window.pollEvent(event)) {
      if (event.type == sf::Event::Closed) {
        window.close();
      } else if (event.type == sf::Event::LostFocus) {
        focus = false;
      } else if (event.type == sf::Event::GainedFocus) {
        focus = true;
      } else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::F2) {
          renderer.show_minimap = !renderer.show_minimap;
        } else if (event.key.code == sf::Keyboard::F3) {
          renderer.debug = !renderer.debug;
        }
      } else if (event.type == sf::Event::Resized) {
        double area = event.size.width * event.size.height;
        sf::Vector2u size(std::sqrt(area) / 12 * 16, std::sqrt(area) / 12 * 9);
        if (size.x != event.size.width) {
          window.setSize(size);
        }
        view.setSize(size.x, size.y);
        view.setCenter(size.x / 2, size.y / 2);
        window.setView(view);
      }
    }

    if (focus) {
      game.update(dt);
      controls.update();
      World *world = game.getWorld();
      if (world) {
        audio.setListener(world->camera);
        Physics::apply_physics(*world, dt);
        renderer.update(*world, world->camera, M_PI_2, 240, dt);
        game.render(renderer);
        world->cleanup();
      } else {
        window.clear();
        renderer.drawText({0, 0}, resourceFolder / "fonts" / "font.ttf",
                          "No world loaded!", 100, {255, 255, 255, 255});
      }
      window.display();
    }
  }
}
