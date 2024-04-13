#include "Renderer.h"
#include "Physics.h"
#include <execution>
#include <boost/range/irange.hpp>
#include <boost/lockfree/queue.hpp>
#include <memory>
#include <format>

sf::Sprite* Renderer::get_column(sf::Texture* texture, Vertex& vertex, sf::Vector2f& collision, int cols) {
	sf::Sprite* sprite = new sf::Sprite(*texture); 
	double scale = texture->getSize().x / vertex.length();
	double left = sqrt(pow(collision.x - vertex.start.x, 2) + pow(collision.y - vertex.start.y, 2));
	sprite->setTextureRect(sf::IntRect(left * scale, 0, cols, texture->getSize().y));
	return sprite;
}

void Renderer::post() {

}

void Renderer::draw_minimap(World& world, Player& camera) {
	//the range of the minimap:
	double range = 5 * METER;
	double on_screen_radius = 100;
	double scale = on_screen_radius / range;
	sf::Vector2f center(window->getSize().x - on_screen_radius - 15, on_screen_radius + 15);

	sf::CircleShape dish(on_screen_radius);
	dish.setOutlineColor(sf::Color::White);
	dish.setOutlineThickness(1);
	dish.setFillColor(sf::Color::Black);
	dish.setPosition(center - sf::Vector2f(on_screen_radius, on_screen_radius));
	window->draw(dish);

	sf::Vector2f pos = Physics::squash(camera.location);
	for (const std::unique_ptr<Vertex>& vertex : world.verticies) {
		if (Physics::distance(pos, vertex->start) < range && Physics::distance(pos, vertex->start) < range) {
			sf::Vertex line[] = {
				sf::Vertex(Physics::mult(Physics::scale(vertex->start - pos, scale), sf::Vector2f(1, -1)) + center),
				sf::Vertex(Physics::mult(Physics::scale(vertex->end - pos, scale), sf::Vector2f(1, -1)) + center)
			};

			window->draw(line, 2, sf::Lines);
		}
	}

	for (const std::unique_ptr<Entity>& entity : world.entities) {
		sf::Vector2f entity_pos = Physics::squash(entity->location);
		if (Physics::distance(pos, entity_pos) < range) {
			double raidus = entity->radius * scale;
			sf::CircleShape indicator(raidus);
			indicator.setFillColor(sf::Color::Red);
			indicator.setPosition(Physics::mult(Physics::scale(entity_pos - pos, scale), sf::Vector2f(1, -1)) + center - sf::Vector2f(raidus, raidus));
			window->draw(indicator);
		}
	}

	if (debug) {
		sf::Vertex vel[] = {
				sf::Vertex(center),
				sf::Vertex(Physics::mult(Physics::scale(camera.velocity, scale), sf::Vector2f(1, -1)) + center)
		};
		vel[0].color = sf::Color::Cyan;
		vel[1].color = sf::Color::Green;
		window->draw(vel, 2, sf::Lines);
	}

	double raidus = camera.radius * scale;
	sf::CircleShape cursor(raidus);
	cursor.setFillColor(sf::Color::Blue);
	cursor.setPosition(center - sf::Vector2f(raidus, raidus));
	window->draw(cursor);
}

sf::Text Renderer::text_of(std::string text) {
	sf::Text output;
	output.setFont(font);
	output.setString(text);
	return output;
}

Renderer::Renderer(sf::RenderWindow& window) {
	this->window = &window;
	font.loadFromFile("resources\\font.ttf");
	debug = false;
}

void Renderer::update(World& world, Player& camera, double fov, double rays) {
	window->clear();

	sf::Sprite sky;
	sky.setTexture(*world.load_texture(world.sky_texture));
	sky.setScale(double(window->getSize().x) / sky.getTexture()->getSize().x, (window->getSize().y / 2.0)/sky.getTexture()->getSize().y);
	window->draw(sky);
	sf::Sprite ground;
	ground.setTexture(*world.load_texture(world.ground_texture));
	ground.setScale(double(window->getSize().x) / ground.getTexture()->getSize().x, (window->getSize().y / 2.0) / ground.getTexture()->getSize().y);
	ground.setPosition(0, window->getSize().y / 2);
	window->draw(ground);

	double offset = fov / rays;
	double xoffset = window->getSize().x / rays;
	boost::integer_range<int> ops = boost::irange<int>((int) ( - rays / 2), (int) (rays / 2));
	boost::lockfree::queue<sf::Sprite*> sprite_queue(0);

	std::for_each(std::execution::par_unseq, ops.begin(), ops.end(),
		[&](const int& i) {
			double angle = Physics::scale_angle(offset * i + camera.location.z);

			std::vector<CastResult> hits = Physics::cast_ray(world, camera.location, angle);
			if (hits.size()) {
				for (int j = hits.size() - 1; j >= 0; j--) {
					CastResult& closest = hits[j];
					if (closest.entity == &camera) {
						continue;
					}
					sf::Texture* texture = world.load_texture(closest.vertex->texture);
					if (texture) {
						sf::Sprite* sprite = get_column(texture, *closest.vertex, closest.point, std::ceil(xoffset));
						double trueDistance = closest.distance * cos(offset * i);
						double vScale = (closest.vertex->height / trueDistance) * (((double)window->getSize().y) / sprite->getTextureRect().height);
						double height = window->getSize().y / 2 - sprite->getTextureRect().height / 2 * vScale;
						double dist = (METER - closest.vertex->height - closest.vertex->z) / (2 * trueDistance) * window->getSize().y;
						sprite->setScale(1, vScale);
						sprite->setPosition(sf::Vector2f(-i * xoffset + window->getSize().x / 2 - sprite->getTextureRect().width / 2, height + dist));
						sprite_queue.push(sprite);
					}
				}
			}
		}
	);
	while (!sprite_queue.empty()) {
		sf::Sprite* sprite;
		sprite_queue.pop(sprite);
		window->draw(*sprite);
		delete sprite;
	}

	//postprocessing effects
	post();

	//move this later this is just for debug right now
	if (debug) {
		sf::Text title = text_of("Camera Debug");
		title.setFillColor(sf::Color::White);
		title.setStyle(sf::Text::Underlined);
		title.setCharacterSize(24);
		window->draw(title);

		sf::Text pos = text_of(std::format("Position ({}, {})", int(camera.location.x), int(camera.location.y)));
		pos.setFillColor(sf::Color::Cyan);
		pos.setCharacterSize(16);
		pos.setPosition(0, 24);
		window->draw(pos);

		sf::Text vel = text_of(std::format("Velocity ({}, {})", int(camera.velocity.x), int(camera.velocity.y)));
		vel.setFillColor(sf::Color::Blue);
		vel.setCharacterSize(16);
		vel.setPosition(0, 40);
		window->draw(vel);


		sf::Text yaw = text_of(std::format("Yaw {}", int(Physics::to_degrees(camera.location.z))));
		yaw.setFillColor(sf::Color::Green);
		yaw.setCharacterSize(16);
		yaw.setPosition(0, 56);
		window->draw(yaw);
	}

	draw_minimap(world, camera);

	window->display();
}
