#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Vertex.h"

class Entity {
public:
	std::string front;
	std::vector<std::pair<std::string, float>> side;
	sf::Vector3f location;
	sf::Vector2f size;

	std::vector<Vertex> to_vertcies();
};

