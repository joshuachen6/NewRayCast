#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include "ResourceManager.h"

const double PI = 3.14159265;


using namespace sf;
using namespace std;

double scaleAngle(double angle) {
	while (angle > PI) {
		angle -= 2 * PI;
	}

	while (angle < -PI) {
		angle += 2 * PI;
	}

	return angle;
}

vector<pair<TexturedVertex, Vector3f>> castRay(double angle, double FOV, Vector3f player, RenderWindow& window) {
	double slope = tan(angle);
	double b = -slope * player.x + player.y;

	if (Keyboard::isKeyPressed(Keyboard::LShift)) {
		Vertex line[] = {
		Vertex(Vector2f(1280 / 2, 720 / 2), Color::Cyan),
		Vertex(Vector2f(1280 / 2 + cos(angle) * 200, 720 / 2 - sin(angle) * 200), Color::Cyan)
		};
		window.draw(line, 2, sf::Lines);
	}

	vector<pair<TexturedVertex, Vector3f>> hits;

	for (TexturedVertex vertex : ResourceManager::vertecies) {
		double cx, cy;
		if (vertex.start.x != vertex.end.x) {
			double vslope = (vertex.end.y - vertex.start.y) / (vertex.end.x - vertex.start.x);
			bool collides = (slope - vslope) != 0;
			if (!collides) {
				continue;
			}
			double vb = -vslope * vertex.start.x + vertex.start.y;
			cx = (vb - b) / (slope - vslope);
			cy = slope * cx + b;
		} else {
			cy = slope * vertex.start.x + b;
			cx = vertex.start.x;
		}

		bool yrange = (cy - min(vertex.start.y, vertex.end.y)) > -0.5 && (cy - max(vertex.start.y, vertex.end.y)) < 0.5;
		bool xrange = (cx - min(vertex.start.x, vertex.end.x)) > -0.5 && (cx - max(vertex.start.x, vertex.end.x)) < 0.5;

		if (!(yrange && xrange)) {
			continue;
		}

		if (abs(scaleAngle((atan2((cy - player.y), (cx - player.x)) - player.z))) > FOV / 2) {
			continue;
		}

		double distance = sqrt(pow(cx - player.x, 2) + pow(cy - player.y, 2));
		hits.push_back(pair<TexturedVertex, Vector3f>(vertex, Vector3f(cx, cy, distance)));

		if (Keyboard::isKeyPressed(Keyboard::LShift)) {
			Vertex line[] = {
				Vector2f(1280 / 2, 720 / 2),
				Vector2f(cx - player.x + 1280 / 2, 720 / 2 - (cy - player.y))
			};
			window.draw(line, 2, sf::Lines);
		}
	}

	sort(hits.begin(), hits.end(), [](pair<TexturedVertex, Vector3f> a, pair<TexturedVertex, Vector3f> b) {
		return a.second.z < b.second.z;
	});

	return hits;
}

int main() {

	Texture texture, texture1, texture2;
	texture.loadFromFile("C:\\Users\\JC200\\Downloads\\wall.jpg");
	texture1.loadFromFile("C:\\Users\\JC200\\Downloads\\wall1.jpg");
	texture2.loadFromFile("C:\\Users\\JC200\\Downloads\\wall2.jpg");
	ResourceManager::addTexture(&texture);
	ResourceManager::addTexture(&texture1);
	ResourceManager::addTexture(&texture2);
	ResourceManager::addVertex(TexturedVertex(Vector2f(100, 50), Vector2f(100, -50), 0));
	ResourceManager::addVertex(TexturedVertex(Vector2f(200, 50), Vector2f(200, -50), 1));
	ResourceManager::addVertex(TexturedVertex(Vector2f(100, -50), Vector2f(200, -50), 2));

	Vector3f player(0, 0, 0);

	double FOV = 3.14159265 / 2;
	int rays = 180;

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
				vector<pair<TexturedVertex, Vector3f>> hits = castRay(player.z, FOV, player, window);
				if (hits.size()) {
					int id = hits[0].first.ID;
					for (int i = 0; i < ResourceManager::vertecies.size(); i++) {
						if (ResourceManager::vertecies[i].ID == id) {
							ResourceManager::vertecies.erase(ResourceManager::vertecies.begin() + i, ResourceManager::vertecies.begin() + i + 1);
							break;
						}
					}
				}
			} else if (event.type == sf::Event::KeyPressed && event.key.code == Keyboard::F) {
				double perp = player.z + PI/2;
				double xoff = cos(perp) * 50;
				double yoff = sin(perp) * 50;
				srand(time(0));
				ResourceManager::addVertex(TexturedVertex(Vector2f(player.x + xoff, player.y + yoff), Vector2f(player.x - xoff, player.y - yoff), ((double) rand())/RAND_MAX * 3));
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

			vector<pair<TexturedVertex, Vector3f>> hits = castRay(angle, FOV, player, window);
			
			if (hits.size()) {
				Sprite sprite = ResourceManager::subTexture(hits[0].first, Vector2f(hits[0].second.x, hits[0].second.y), xoffset);
				double vScale = (12 / pow(hits[0].second.z, 0.75)) * (720.0 / sprite.getTextureRect().height);
				sprite.setScale(1, vScale);
				sprite.setPosition(Vector2f(-i * xoffset + 1280 / 2 - sprite.getTextureRect().width / 2, 720 / 2 - sprite.getTextureRect().height / 2 * vScale));

				if (!Keyboard::isKeyPressed(Keyboard::LShift))
				window.draw(sprite);
			}
		}

		if (Keyboard::isKeyPressed(Keyboard::LShift)) {
			for (TexturedVertex &vertex : ResourceManager::vertecies) {
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
			targetCircle.setPosition(1280 / 2 + cos(player.z)*5 - 1.5, 720 / 2 - sin(player.z) * 5 - 1.5);
			targetCircle.setRadius(3);
			targetCircle.setFillColor(Color::Yellow);
			window.draw(targetCircle);
		}

		window.display();
	}
}
