#include "World.h"
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <mutex>
#include <iostream>
#include <format>

std::mutex texture_mutex;
std::mutex model_mutex;
std::mutex entity_mutex;

sf::Texture* World::load_texture(const std::string& texture) {
	std::lock_guard<std::mutex> lock(texture_mutex);
	if (!texture_map.contains(texture)) {
		texture_map[texture] = sf::Texture();
		if (texture.empty()) {
			texture_map[texture].loadFromFile("resources\\sprites\\placeholder.png");
		} else {
			texture_map[texture].loadFromFile(texture);
		}
	}
	return &texture_map[texture];
}

const std::vector<Vertex>& World::load_model(const std::string& model) {
	std::lock_guard<std::mutex> lock(model_mutex);
	if (!model_map.contains(model)) {
		std::ifstream file(model);
		if (file.is_open()) {
			model_map[model] = std::vector<Vertex>();
			std::string line;

			std::vector<std::string> data;
			std::vector<std::string> start(2), end(2);

			while (std::getline(file, line)) {
				boost::trim(line);
				boost::erase_all(line, " ");

				boost::split(data, line, boost::is_any_of(";"));

				boost::split(start, data[0], boost::is_any_of(","));
				boost::split(end, data[1], boost::is_any_of(","));

				Vertex vertex(
					sf::Vector2f(std::stod(start[0]), std::stod(start[1])),
					sf::Vector2f(std::stod(end[0]), std::stod(end[1])),
					std::stod(data[2]),
					std::stod(data[3]),
					data[4]
				);
				model_map[model].push_back(vertex);
			}
		} else {
			std::cerr << std::format("Failed to load model {}", model) << std::endl;
		}
		file.close();
	}
	return model_map[model];
}

void World::add_vertex(Vertex* vertex) {
	vertices.push_back(std::unique_ptr<Vertex>(vertex));
}

void World::add_entity(Entity* entity) {
	entities.push_back(std::unique_ptr<Entity>(entity));
}

void World::spawn_model(std::string model, sf::Vector3f position) {
	const std::vector<Vertex>& vertices = load_model(model);
	for (const Vertex& vertex : vertices) {
		Vertex temp = vertex.translated(position);
		this->vertices.push_back(std::make_unique<Vertex>(temp));
	}
}

void World::spawn_entity(std::string entity, sf::Vector3f position) {
	std::lock_guard<std::mutex> lock(entity_mutex);
	if (!entity_map.contains(entity)) {
		std::ifstream file(entity);
		if (file.is_open()) {
			std::unordered_map < std::string, std::string> data;
			std::string line;
			std::vector<std::string> pair(2);

			while (std::getline(file, line)) {
				boost::trim(line);
				boost::erase_all(line, " ");
				boost::split(pair, line, boost::is_any_of(":"));
				data[pair[0]] = pair[1];
			}

			entity_map[entity] = data;

		} else {
			std::cerr << std::format("Failed to load entity {}", entity) << std::endl;
		}
		file.close();
	}

	add_entity(new Entity(entity_map[entity]));
}
