#include "World.h"
#include "Renderer.h"
#include "Vertex.h"
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Physics.h"


int main() {
	sf::RenderWindow window(sf::VideoMode(1280, 720), "Game");

	Renderer renderer(window);

	sf::Vector3f player;

	World world;

	world.verticies.push_back(Vertex(sf::Vector2f(10, -50), sf::Vector2f(10, 50), "C:\\Users\\JC200\\Downloads\\cpp_standard.jpg"));

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
			player.z = Physics::scale_angle(player.z + (4 * M_PI / 7) * dt);
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
			player.z = Physics::scale_angle(player.z - (4 * M_PI / 7) * dt);
		}

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
			player.x += cos(player.z) * 500 * dt;
			player.y += sin(player.z) * 500 * dt;
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
			player.x -= cos(player.z) * 500 * dt;
			player.y -= sin(player.z) * 500 * dt;
		}


		renderer.update(world, player, M_PI_2, 640);
	}
}