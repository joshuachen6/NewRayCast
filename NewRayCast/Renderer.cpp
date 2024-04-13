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

Renderer::Renderer(sf::RenderWindow& window) {
	this->window = &window;
	font.loadFromFile("resources\\font.ttf");
}

void Renderer::update(World& world, Player& camera, double fov, double rays) {
	window->clear();
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

	//move this later this is just for debug right now
	sf::Text pos;
	pos.setFont(font);
	pos.setString(std::format("{}, {}, {}", int(camera.location.x), int(camera.location.y), int(camera.location.z)));
	pos.setFillColor(sf::Color::White);
	pos.setCharacterSize(32);
	window->draw(pos);

	sf::Text vel;
	vel.setFont(font);
	vel.setString(std::format("{}, {}", int(camera.velocity.x), int(camera.velocity.y)));
	vel.setFillColor(sf::Color::Cyan);
	vel.setCharacterSize(32);
	vel.setPosition(0, 32);

	window->draw(vel);

	post();

	window->display();
}
