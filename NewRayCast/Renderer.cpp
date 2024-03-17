#include "Renderer.h"
#include "Physics.h"

sf::Sprite Renderer::get_column(sf::Texture* texture, Vertex& vertex, sf::Vector2f& collision, int cols) {
	sf::Sprite sprite(*texture); 
	double scale = texture->getSize().x / vertex.length();
	double left = sqrt(pow(collision.x - vertex.start.x, 2) + pow(collision.y - vertex.start.y, 2));
	sprite.setTextureRect(sf::IntRect(left * scale, 0, cols, texture->getSize().y));
	return sprite;
}

Renderer::Renderer(sf::RenderWindow& window) {
	this->window = &window;
}

void Renderer::update(World& world, sf::Vector3f& camera, double fov, double rays) {
	window->clear();
	double offset = fov / rays;
	double xoffset = window->getSize().x / rays;
	for (int i = -rays / 2; i < rays / 2; i++) {
		double angle = Physics::scale_angle(offset * i + camera.z);

		std::vector<CastResult> hits = Physics::cast_ray(world, camera, angle);
		if (hits.size()) {
			for (int j = hits.size() - 1; j >= 0; j--) {
				CastResult& closest = hits[j];
				sf::Texture* texture = world.load_texture(closest.vertex->texture);
				if (texture) {
					sf::Sprite sprite = get_column(texture, *closest.vertex, closest.point, std::ceil(xoffset));
					double trueDistance = closest.distance * cos(offset * i);
					double vScale = (closest.vertex->height / trueDistance) * (((double) window->getSize().y) / sprite.getTextureRect().height);
					double height = window->getSize().y / 2 - sprite.getTextureRect().height / 2 * vScale;
					double dist = (METER - closest.vertex->height - closest.vertex->z)/ (2 * trueDistance) * window->getSize().y;
					sprite.setScale(1, vScale);
					sprite.setPosition(sf::Vector2f(-i * xoffset + window->getSize().x / 2 - sprite.getTextureRect().width / 2, height+dist));
					window->draw(sprite);
				}
				if (closest.entity) {
					delete closest.vertex;
				}
			}
		}
	}
	window->display();
}
