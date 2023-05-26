#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include "ResourceManager.h"
#include "CastUtils.h"


using namespace sf;
using namespace std;



int main() {

	Texture texture, texture1, texture2, texture3;
	texture.loadFromFile("C:\\Users\\JC200\\Downloads\\wall.jpg");
	texture1.loadFromFile("C:\\Users\\JC200\\Downloads\\wall1.jpg");
	texture2.loadFromFile("C:\\Users\\JC200\\Downloads\\wall2.jpg");
	texture3.loadFromFile("C:\\Users\\JC200\\Downloads\\wall3.jpg");
	ResourceManager::addTexture(&texture);
	ResourceManager::addTexture(&texture1);
	ResourceManager::addTexture(&texture2);
	ResourceManager::addTexture(&texture3);
	ResourceManager::addVertex(TexturedVertex(Vector2f(100, 50), Vector2f(100, -50), 0));
	ResourceManager::addVertex(TexturedVertex(Vector2f(200, 50), Vector2f(200, -50), 1));
	ResourceManager::addVertex(TexturedVertex(Vector2f(100, -50), Vector2f(200, -50), 2));
	ResourceManager::addVertex(TexturedVertex(Vector2f(100, 50), Vector2f(200, 50), 3));

	Vector3f player(0, 0, 0);

	double FOV = 3.14159265 / 2;
	int rays = 120;

	chrono::time_point<chrono::system_clock> last = chrono::system_clock::now();

	sf::RenderWindow window(sf::VideoMode(1280, 720), "TestProject");
	while (window.isOpen()) {
		double dt = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - last).count()/1000.0;
		last = chrono::system_clock::now();

		//cout << 1 / dt << endl;;


		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			} else if (event.type == sf::Event::KeyPressed && event.key.code == Keyboard::Space) {
				vector<CastResult> hits = castRay(player.z, FOV, player);
				if (hits.size()) {
					int id = hits[0].vertex.ID;
					ResourceManager::removeVertex(id);
				}
			} else if (event.type == sf::Event::KeyPressed && event.key.code == Keyboard::F) {
				double perp = player.z + PI/2;
				double xoff = cos(perp) * 50;
				double yoff = sin(perp) * 50;
				srand(chrono::system_clock::now().time_since_epoch().count());
				double random = ((double)rand()) / RAND_MAX;
				cout << random << endl;
				ResourceManager::addVertex(TexturedVertex(Vector2f(player.x + xoff, player.y + yoff), Vector2f(player.x - xoff, player.y - yoff), random * 3));
			}
		}


		//handle input
		if (Keyboard::isKeyPressed(Keyboard::Left)) {
			player.z = scaleAngle(player.z + (4 * PI / 7) * dt);
		} else if (Keyboard::isKeyPressed(Keyboard::Right)) {
			player.z = scaleAngle(player.z - (4 * PI / 7) * dt);
		} 
		
		if (Keyboard::isKeyPressed(Keyboard::Up)) {
			player.x += cos(player.z) * 45 * dt;
			player.y += sin(player.z) * 45 * dt;
		} else if (Keyboard::isKeyPressed(Keyboard::Down)) {
			player.x -= cos(player.z) * 45 * dt;
			player.y -= sin(player.z) * 45 * dt;
		}

		window.clear();

		//cast rays
		double offset = FOV / rays;
		double xoffset = 1280 / rays;
		for (int i = -rays/2; i < rays/2; i++) {
			double angle = scaleAngle(offset * i + player.z);

			vector<CastResult> hits = castRay(angle, FOV, player);
			
			if (hits.size()) {
				//debug
				if (!Keyboard::isKeyPressed(Keyboard::LShift)) {
					Sprite sprite = ResourceManager::subTexture(hits[0].vertex, Vector2f(hits[0].cx, hits[0].cy), xoffset);
					double trueDistance = hits[0].distance * cos(offset * i);
					double vScale = (12 / pow(trueDistance, 0.75)) * (720.0 / sprite.getTextureRect().height);
					sprite.setScale(1, vScale);
					sprite.setPosition(Vector2f(-i * xoffset + 1280 / 2 - sprite.getTextureRect().width / 2, 720 / 2 - sprite.getTextureRect().height / 2 * vScale));
					window.draw(sprite);
				} else {
					Vertex line[] = {
						Vector2f(1280 / 2, 720 / 2),
						Vector2f(hits[0].cx - player.x + 1280 / 2, 720 / 2 - (hits[0].cy - player.y))
					};
					window.draw(line, 2, sf::Lines);

					for (TexturedVertex& vertex : ResourceManager::vertecies) {
						Vertex line[] = {
							Vertex(Vector2f(vertex.start.x - player.x + 1280 / 2, 720 / 2 - (vertex.start.y - player.y)), Color::Green),
							Vertex(Vector2f(vertex.end.x - player.x + 1280 / 2, 720 / 2 - (vertex.end.y - player.y)), Color::Green)
						};
						window.draw(line, 2, sf::Lines);
					}
					CircleShape playerCircle;
					playerCircle.setPosition(1280 / 2 - 2.5, 720 / 2 - 2.5);
					playerCircle.setRadius(5);
					playerCircle.setFillColor(Color::Red);
					window.draw(playerCircle);

					CircleShape targetCircle;
					targetCircle.setPosition(1280 / 2 + cos(player.z) * 5 - 1.5, 720 / 2 - sin(player.z) * 5 - 1.5);
					targetCircle.setRadius(3);
					targetCircle.setFillColor(Color::Yellow);
					window.draw(targetCircle);
				}
			}
		}
		window.display();
	}
}
