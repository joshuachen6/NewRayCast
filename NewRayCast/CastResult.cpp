#include "CastResult.h"

CastResult::CastResult(sf::Vector2f point, double distance, Vertex* vertex, Entity* entity) {
	this->vertex = vertex;
	this->entity = entity;
	this->point = point;
	this->distance = distance;
	if (entity) {
		temp_vertex = std::shared_ptr<Vertex>(vertex);
	}
}

CastResult::CastResult(const CastResult& other) {
	vertex = other.vertex;
	entity = other.entity;
	point = other.point;
	distance = other.distance;
	temp_vertex = other.temp_vertex;
}
