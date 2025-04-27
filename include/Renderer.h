#pragma once
#include <SFML/Graphics.hpp>
#include "Vertex.h"
#include "Entity.h"
#include "World.h"
#include <vector>
#include "CastResult.h"
#include "Player.h"

class Renderer {
private:
	sf::RenderWindow* window;
	sf::RenderTexture render_texture;
	sf::Sprite* get_column(sf::Texture* texture, Vertex& vertex, sf::Vector2f& collision, int cols);
	sf::Font font;
	sf::Shader noise_shader;

	void draw_minimap(World& world, Player& player);

	sf::Text text_of(std::string text);

public:
	bool debug;
	bool show_minimap;
	Renderer(sf::RenderWindow& window);
	void update(World& world, Player& camera, double fov, double rays, double dt);
};