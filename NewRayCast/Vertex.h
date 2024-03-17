#pragma once
#include <SFML/Graphics.hpp>

class Vertex {
public:
	sf::Vector2f start, end;
	double height;
	double z;
	std::string texture;

	Vertex(sf::Vector2f start, sf::Vector2f end, double height, double z, std::string texture);
	Vertex(const Vertex& other);
	Vertex translated(sf::Vector2f translate, double rotation=0) const;
	Vertex translated(sf::Vector3f translate) const;
	double length() const;
};

