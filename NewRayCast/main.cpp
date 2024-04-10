#include "World.h"
#include "Renderer.h"
#include "Vertex.h"
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Physics.h"
#include "Player.h"
#include <iostream>

//Test class
class DumbModel : public Entity {
public:
	DumbModel() {
		model = "C:\\Users\\JC200\\Downloads\\model.txt";
		location = sf::Vector3f(100, 100, M_PI_2);
	}
};


int main() {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Game");

	Renderer renderer(window);

	World world;
	world.gravity = 10;

	Player& player = static_cast<Player&>(world.add_entity(Player()));

	world.entities.push_back(DumbModel());

	world.verticies.push_back(Vertex(sf::Vector2f(1000, 1000), sf::Vector2f(1000, 900), 100, 0, "C:\\Users\\Joshua\\Downloads\\duck.jpg"));
	world.verticies.push_back(Vertex(sf::Vector2f(-1000, 1000), sf::Vector2f(-1000, 900), 100, 0, "C:\\Users\\Joshua\\Downloads\\duck1.png"));

	std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();

	while (window.isOpen()) {

		double dt = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now() - last).count() / 1e9;
		last = std::chrono::system_clock::now();

		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		// Movement
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
			player.location.z = Physics::scale_angle(player.location.z + (4 * M_PI / 7) * dt);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player.location.z = Physics::scale_angle(player.location.z - (4 * M_PI / 7) * dt);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			player.velocity.x += cos(player.location.z) * 3 * METER * dt;
			player.velocity.y += sin(player.location.z) * 3 * METER * dt;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			player.velocity.x -= cos(player.location.z) * 3 * METER * dt;
			player.velocity.y -= sin(player.location.z) * 3 * METER * dt;
		}

		Physics::apply_physics(world, dt);
		renderer.update(world, player, M_PI_2, 240);
		std::cout << player.location.z << std::endl;
	}
}