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
	sf::Sprite* get_column(sf::Texture* texture, Vertex& vertex, sf::Vector2f& collision, int cols);
	sf::Font font;
	void post();

public:
	Renderer(sf::RenderWindow& window);
	void update(World& world, Player& camera, double fov, double rays);
};