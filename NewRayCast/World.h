#pragma once
#include <vector>
#include "Entity.h"
#include "Vertex.h"

class World {
public:
	std::vector<Vertex> verticies;
	std::vector<Entity> entities;
};

