#pragma once
#include <SFML/Graphics.hpp>
#include "Vertex.h"
#include "Entity.h"
#include "World.h"
#include <vector>
#include "CastResult.h"
#include <unordered_map>

class Renderer {
private:
	sf::RenderWindow* window;
	sf::Sprite get_column(Vertex& vertex, sf::Vector2f& collision, int cols);
	std::unordered_map<std::string, sf::Texture> textures;

public:
	Renderer(sf::RenderWindow& window);
	void update(World& world, sf::Vector3f& camera, double fov, double rays);
	sf::Texture* load_texture(std::string texture);
};