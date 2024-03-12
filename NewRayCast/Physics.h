#pragma once
#include "World.h"
#include "Entity.h"
#include "Vertex.h"
#include <SFML/System.hpp>
#include <vector>
#include "CastResult.h"


class Physics {
public:
	static std::vector<CastResult> cast_ray(World& world, sf::Vector3f& source, double angle);
	static double scale_angle(double radians);
	static bool hits_vertex(Vertex& vertex, sf::Vector2f& point, sf::Vector3f& source, double angle);
};

