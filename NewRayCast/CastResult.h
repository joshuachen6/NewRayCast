#pragma once
#include <SFML/Graphics.hpp>
#include "Vertex.h"
#include "Entity.h"

class CastResult {
public:
	Vertex* vertex;
	Entity* entity;
	sf::Vector2f point;
	double distance;
	
	CastResult(sf::Vector2f point, double distance, Vertex* vertex=nullptr, Entity* entity=nullptr);
};

