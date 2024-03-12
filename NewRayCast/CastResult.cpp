#include "CastResult.h"

CastResult::CastResult(sf::Vector2f point, double distance, Vertex* vertex, Entity* entity) {
	this->vertex = vertex;
	this->entity = entity;
	this->point = point;
	this->distance = distance;
}