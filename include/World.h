#pragma once
#include <vector>
#include "Entity.h"
#include "Vertex.h"
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <memory>

class World {
private:
	std::unordered_map<std::string, sf::Texture> texture_map;
	std::unordered_map<std::string, std::vector<Vertex>> model_map;
	std::unordered_map<std::string, std::unordered_map<std::string, std::string>> entity_map;
public:
	std::vector<std::unique_ptr<Vertex>> vertices;
	std::vector<std::unique_ptr<Entity>> entities;

	double friction;
	double gravity;
	std::string sky_texture;
	std::string ground_texture;

	sf::Texture* load_texture(const std::string& texture);
	const std::vector<Vertex>& load_model(const std::string& model);

	void add_vertex(Vertex* vertex);
	void add_entity(Entity* entity);

	void spawn_model(std::string model, sf::Vector3f position);
	void spawn_entity(std::string entity, sf::Vector3f position);

	void clear_cache();
};

