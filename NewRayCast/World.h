#pragma once
#include <vector>
#include "Entity.h"
#include "Vertex.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <memory>

class World {
private:
	std::unordered_map<std::string, sf::Texture> textures;
	std::unordered_map<std::string, std::vector<Vertex>> models;
public:
	std::vector<std::unique_ptr<Vertex>> verticies; //Also this is spelled wrong
	std::vector<std::unique_ptr<Entity>> entities;

	double friction;
	double gravity;

	sf::Texture* load_texture(const std::string& texture);
	const std::vector<Vertex>& load_model(const std::string& model);

	void add_vertex(Vertex* vertex);
	void add_entity(Entity* entity);
};

