#include "Renderer.h"
#include "Physics.h"

sf::Sprite Renderer::get_column(Vertex& vertex, sf::Vector2f& collision, int cols) {
	sf::Texture* texture = load_texture(vertex.texture);
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
				sf::Sprite sprite = get_column(*closest.vertex, closest.point, std::ceil(xoffset));
				double trueDistance = closest.distance * cos(offset * i);
				double vScale = (M / trueDistance) * (720.0 / sprite.getTextureRect().height);
				sprite.setScale(1, vScale);
				sprite.setPosition(sf::Vector2f(-i * xoffset + window->getSize().x / 2 - sprite.getTextureRect().width / 2, window->getSize().y / 2 - sprite.getTextureRect().height / 2 * vScale));
				window->draw(sprite);
			}
		}
	}
	window->display();
}

sf::Texture* Renderer::load_texture(std::string texture) {
	if (!textures.contains(texture)) {
		textures[texture] = sf::Texture();
		textures[texture].loadFromFile(texture);
	}
	return &textures[texture];
}
