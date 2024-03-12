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
	double xoffset = 1280 / rays;
	for (int i = -rays / 2; i < rays / 2; i++) {
		double angle = Physics::scale_angle(offset * i + camera.z);

		std::vector<CastResult> hits = Physics::cast_ray(world, camera, angle);
		if (hits.size()) {
			CastResult& closest = hits[0];
			sf::Sprite sprite = get_column(*closest.vertex, closest.point, xoffset);
			double trueDistance = hits[0].distance * cos(offset * i);
			double vScale = (12 / pow(trueDistance, 0.75)) * (720.0 / sprite.getTextureRect().height);
			sprite.setScale(1, vScale);
			sprite.setPosition(sf::Vector2f(-i * xoffset + 1280 / 2 - sprite.getTextureRect().width / 2, 720 / 2 - sprite.getTextureRect().height / 2 * vScale));
			window->draw(sprite);
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
