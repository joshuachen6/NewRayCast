#pragma once
#include <SFML/Graphics.hpp>
#include "Vertex.h"
#include "Entity.h"
#include "World.h"
#include <vector>
#include "CastResult.h"

class Renderer {
private:
	sf::RenderWindow* window;
	sf::Sprite* get_column(sf::Texture* texture, Vertex& vertex, sf::Vector2f& collision, int cols);

public:
	Renderer(sf::RenderWindow& window);
	void update(World& world, sf::Vector3f& camera, double fov, double rays);
};