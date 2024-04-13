#include "World.h"
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>
#include <mutex>
#include <iostream>
#include <format>

std::mutex texture_mutex;
std::mutex model_mutex;

sf::Texture* World::load_texture(const std::string& texture) {
	std::lock_guard<std::mutex> lock(texture_mutex);
	if (!textures.contains(texture)) {
		textures[texture] = sf::Texture();
		if (texture.empty()) {
			textures[texture].loadFromFile("resources\\sprites\\placeholder.png");
		} else {
			textures[texture].loadFromFile(texture);
		}
	}
	return &textures[texture];
}

const std::vector<Vertex>& World::load_model(const std::string& model) {
	std::lock_guard<std::mutex> lock(model_mutex);
	if (!models.contains(model)) {
		std::ifstream file(model);
		if (file.is_open()) {
			models[model] = std::vector<Vertex>();
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
				models[model].push_back(vertex);
			}
		} else {
			std::cerr << std::format("Failed to load model {}", model) << std::endl;
		}
		file.close();
	}
	return models[model];
}

void World::add_vertex(Vertex* vertex) {
	verticies.push_back(std::unique_ptr<Vertex>(vertex));
}

void World::add_entity(Entity* entity) {
	entities.push_back(std::unique_ptr<Entity>(entity));
}
