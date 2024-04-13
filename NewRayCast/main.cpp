#include "World.h"
#include "Renderer.h"
#include "Vertex.h"
#include <chrono>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Physics.h"
#include "Player.h"

//Test class
class DumbModel : public Entity {
public:
	DumbModel() {
		model = "C:\\Users\\JC200\\Downloads\\model.txt";
		location = sf::Vector3f(100, 100, M_PI_2);
		radius = 15;
		mass = 10;
	}
};


int main() {

	//is the game in focus
	bool focus = true;

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Game");

	Renderer renderer(window);

	World world;
	world.gravity = 10;
	world.friction = 0.35;
	world.sky_texture = "resources\\sprites\\sky.jpg";
	world.ground_texture = "resources\\sprites\\grass.jpg";

	Player* player = new Player();
	world.add_entity(player);

	world.add_entity(new DumbModel());

	world.add_vertex(new Vertex(sf::Vector2f(1000, 1000), sf::Vector2f(1000, 900), 100, 0, "resources\\sprites\\placeholder.png"));
	world.add_vertex(new Vertex(sf::Vector2f(0, 1000), sf::Vector2f(0, 900), 100, 0, "resources\\sprites\\placeholder.png"));
	world.add_vertex(new Vertex(sf::Vector2f(-1000, 1000), sf::Vector2f(-1000, 900), 100, 0, "resources\\sprites\\placeholder.png"));

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
				if (event.key.code == sf::Keyboard::F3) {
					renderer.debug = !renderer.debug;
				}
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
				if (Physics::mag(player->velocity) < 2.5 * METER) {
					player->velocity.x += cos(player->location.z) * 3.5 * METER * dt;
					player->velocity.y += sin(player->location.z) * 3.5 * METER * dt;
				}
			} else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
				if (Physics::mag(player->velocity) < 2.5 * METER) {
					player->velocity.x -= cos(player->location.z) * 3.5 * METER * dt;
					player->velocity.y -= sin(player->location.z) * 3.5 * METER * dt;
				}
			}

			Physics::apply_physics(world, dt);
			renderer.update(world, *player, M_PI_2, 240);
		}
	}
}