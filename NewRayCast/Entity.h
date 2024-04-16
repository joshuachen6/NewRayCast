#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <unordered_map>
#include "Vertex.h"

class Entity {
public:
	std::string model;
	std::string script;
	sf::Vector3f location;
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	double mass;
	double radius;
	bool is_static;

	Entity() = default;
	Entity(std::unordered_map<std::string, std::string>& data, sf::Vector3f location=sf::Vector3f());
};

