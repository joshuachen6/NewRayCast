#include "Renderer.h"
#include "Vertex.h"
#include "World.h"
#include <chrono>
#define _USE_MATH_DEFINES
#include "Physics.h"
#include "Player.h"
#include <filesystem>
#include <math.h>

// Test class
class DumbModel : public Entity {
public:
  DumbModel() {
    model = "./resources/models/day_tree.txt";
    location = sf::Vector3f(100, 0, M_PI_2);
    radius = 50;
    mass = 10;
  }
};

int main() {
  // get the folder for the resources
  std::filesystem::path resourceFolder("resources");

  // is the game in focus
  bool focus = true;

  sf::RenderWindow window(sf::VideoMode(1280, 720), "Game");
  sf::View view(sf::FloatRect(0, 0, 1280, 720));
  window.setView(view);

  Renderer renderer(window);

  World world;
  world.gravity = 10;
  world.friction = 0.35;
  world.sky_texture = resourceFolder / "sprites" / "sky.jpg";
  world.ground_texture = resourceFolder / "sprites" / "grass.jpg";

  Player *player = new Player();
  world.add_entity(player);

  world.add_entity(new DumbModel());

  // test spawn some trees
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < 5; j++) {
      world.spawn_model(resourceFolder / "models" / "day_tree.txt", sf::Vector3f(250 + i * 150, j * 150, 0));
    }
  }

  std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();

  while (window.isOpen()) {

    double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - last).count() / 1e9;
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
        if (event.key.code == sf::Keyboard::F1) {
          world.clear_cache();
        } else if (event.key.code == sf::Keyboard::F2) {
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
      // Movement
      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        player->location.z = Physics::scale_angle(player->location.z + (4 * M_PI / 7) * dt);
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        player->location.z = Physics::scale_angle(player->location.z - (4 * M_PI / 7) * dt);
      }

      if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        if (Physics::mag(player->velocity) < 2 * METER) {
          player->velocity.x += cos(player->location.z) * 3.5 * METER * dt;
          player->velocity.y += sin(player->location.z) * 3.5 * METER * dt;
        }
      } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        if (Physics::mag(player->velocity) < 2 * METER) {
          player->velocity.x -= cos(player->location.z) * 3.5 * METER * dt;
          player->velocity.y -= sin(player->location.z) * 3.5 * METER * dt;
        }
      }

      Physics::apply_physics(world, dt);
      renderer.update(world, *player, M_PI_2, 240, dt);
    }
  }
}
