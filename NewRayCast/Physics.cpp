#include "Physics.h"
#define _USE_MATH_DEFINES
#include <math.h>

std::vector<CastResult> Physics::cast_ray(World& world, sf::Vector3f& source, double angle) {
	std::vector<CastResult> hits;

	for (Vertex* vertex : world.verticies) {
		sf::Vector2f hit;
		if (hits_vertex(*vertex, hit, source, angle)) {
			double distance = sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
			hits.push_back(CastResult(sf::Vector2f(hit.x, hit.y), distance, vertex));
		}
	}

	for (Entity* entity : world.entities) {
		sf::Vector2f hit;
		const std::vector<Vertex>& verticies = world.load_model(entity->model);
		for (const Vertex& vertex : verticies) {
			Vertex temp = vertex.translated(entity->location);
			Vertex* transformed = new Vertex(temp);
			if (hits_vertex(*transformed, hit, source, angle)) {
				double distance = sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
				hits.push_back(CastResult(sf::Vector2f(hit.x, hit.y), distance, transformed, entity));
			}
			else {
				delete transformed;
			}
		}
	}

	sort(hits.begin(), hits.end(), [](CastResult a, CastResult b) {
		return a.distance < b.distance;
		});

	return hits;
}

double Physics::scale_angle(double radians) {
	while (radians > M_PI) {
		radians -= 2 * M_PI;
	}

	while (radians < -M_PI) {
		radians += 2 * M_PI;
	}

	return radians;
}

bool Physics::hits_vertex(Vertex& vertex, sf::Vector2f& point, sf::Vector3f& source, double angle) {
	double slope = tan(angle);
	double b = source.y - source.x * slope;
	double cx, cy;
	if (vertex.start.x != vertex.end.x) {
		double vslope = (vertex.end.y - vertex.start.y) / (vertex.end.x - vertex.start.x);
		bool collides = (slope - vslope) != 0;
		if (!collides) {
			return false;
		}
		double vb = -vslope * vertex.start.x + vertex.start.y;
		cx = (vb - b) / (slope - vslope);
		cy = slope * cx + b;
	}
	else {
		cy = slope * vertex.start.x + b;
		cx = vertex.start.x;
	}

	bool yrange = (cy - std::min(vertex.start.y, vertex.end.y)) > -0.5 && (cy - std::max(vertex.start.y, vertex.end.y)) < 1;
	bool xrange = (cx - std::min(vertex.start.x, vertex.end.x)) > -0.5 && (cx - std::max(vertex.start.x, vertex.end.x)) < 1;

	if (!(yrange && xrange)) {
		return false;
	}

	if (std::abs(scale_angle((atan2((cy - source.y), (cx - source.x)) - source.z))) >= M_PI_2) {
		return false;
	}

	point.x = cx;
	point.y = cy;

	return true;
}