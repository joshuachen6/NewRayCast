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
	radians = fmod(radians, 2 * M_PI);
	if (radians > M_PI) {
		radians -= 2 * M_PI;
	} else if (radians < -M_PI) {
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

void Physics::apply_physics(World& world, double dt) {
	for (Entity& entity : world.entities) {
		entity.velocity += scale(entity.acceleration, dt);

		sf::Vector2f friction = scale(normalize(entity.velocity), world.friction * entity.mass * world.gravity * dt);
		if (mag(friction) > mag(entity.velocity)) {
			entity.velocity = sf::Vector2f();
		} else {
			entity.velocity -= friction;
		}

		std::vector<CastResult> potential_hits;
		while ((potential_hits = cast_ray(world, entity.location, direction(entity.velocity))).size()) {
			CastResult& result = potential_hits[0];
			if (result.distance > mag(entity.velocity)) {
				break;
			}
			if (result.entity) {
				sf::Vector2f total_momentum = scale(entity.velocity, entity.mass) + scale(result.entity->velocity, result.entity->mass);
				sf::Vector2f new_velocity = scale(total_momentum, 1 / (entity.mass + result.entity->mass));
				entity.velocity = new_velocity;
				result.entity->velocity = new_velocity;
			}
			sf::Vector2f wall_tangent = normalize(result.vertex->start - result.vertex->end);
			entity.velocity = scale(normalize(entity.velocity), dot(wall_tangent, entity.velocity));
			/*
			TODO:
			
			Issue:
			There is a pretty significant issue with this code here, it essentially means that you can only move "along" the wall.
			This means that there is no way to "approach" the wall as your movement will always be defected along the wall.
			
			Solution:
			The solution is to also include the normal component of the wall. This way you can both go towards the wall and along the wall.
			First limit the component that is normal with the wall so that you can only go a certain amount towards the wall.
			Then add the tangential component.
			*/

		}
		entity.location = sf::Vector3f(entity.location.x + entity.velocity.x * dt, entity.location.y + entity.velocity.y * dt, entity.location.z);
	}
}

double Physics::dot(sf::Vector2f a, sf::Vector2f b) {
	return a.x * b.x + a.y * b.y;
}

sf::Vector2f Physics::scale(sf::Vector2f vec, double scalar) {
	return sf::Vector2f(vec.x * scalar, vec.y * scalar);
}

sf::Vector2f Physics::normalize(sf::Vector2f vec) {
	if (vec.x == 0 && vec.y == 0) {
		return vec;
	}
	double magnitude = mag(vec);
	return sf::Vector2f(vec.x/magnitude, vec.y/magnitude);
}

double Physics::mag(sf::Vector2f vec) {
	return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

double Physics::direction(sf::Vector2f vec) {
	return scale_angle(std::atan2(vec.x, vec.y));
}
