#pragma once
#include <SFML/Graphics.hpp>

class Vertex {
public:
	sf::Vector2f start, end;
	std::string texture;

	Vertex(sf::Vector2f start, sf::Vector2f end, std::string texture);

	double length();
};

