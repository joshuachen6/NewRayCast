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
	std::vector<Vertex> verticies; //Also this is spelled wrong
	std::vector<Entity> entities;
	/*
	TODO:

	Problem:
	Because verticies and entities are stored as values this means that they cannot do polymorphism.
	When you store a subclass as the base class, it "converts" it to the base class and you cannot convert it back.

	Solution:
	Convert these to pointers so that you can cast them to the polymorphic subclasses.
	*/

	double friction;
	double gravity;

	sf::Texture* load_texture(const std::string& texture);
	const std::vector<Vertex>& load_model(const std::string& model);
	Vertex& add_vertex(const Vertex& vertex);
	Entity& add_entity(const Entity& entity);
};

