#pragma once
#include "World.h"
#include "Entity.h"
#include "Vertex.h"
#include <SFML/System.hpp>
#include <vector>
#include "CastResult.h"
#include <unordered_map>

const int METER = 100;


class Physics {
public:
	static std::vector<CastResult> cast_ray(World& world, sf::Vector3f& source, double angle);
	static double scale_angle(double radians);
	static bool hits_vertex(Vertex& vertex, sf::Vector2f& point, sf::Vector3f& source, double angle);
	static void apply_physics(World& world, double dt);

	static double dot(sf::Vector2f a, sf::Vector2f b);
	static sf::Vector2f scale(sf::Vector2f vec, double scalar);
	static sf::Vector2f normalize(sf::Vector2f vec);
	static double mag(sf::Vector2f vec);
	static double direction(sf::Vector2f vec);
};

