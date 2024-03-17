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
private:
	std::unordered_map<std::string, std::vector<Vertex>> models;
public:
	static std::vector<CastResult> cast_ray(World& world, sf::Vector3f& source, double angle);
	static double scale_angle(double radians);
	static bool hits_vertex(Vertex& vertex, sf::Vector2f& point, sf::Vector3f& source, double angle);
};

