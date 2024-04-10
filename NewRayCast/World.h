#pragma once
#include <vector>
#include "Entity.h"
#include "Vertex.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>

class World {
private:
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, std::vector<Vertex>> models;
public:
	std::vector<Vertex*> verticies;
	std::vector<Entity*> entities;

	sf::Texture* load_texture(std::string texture);
	const std::vector<Vertex>& load_model(std::string model);
};

