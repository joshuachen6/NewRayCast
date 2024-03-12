#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "Vertex.h"

class Entity {
public:
	std::string front;
	std::string side;
	sf::Vector3f location;
	double radius;

	std::vector<Vertex> to_vertcies();
};

