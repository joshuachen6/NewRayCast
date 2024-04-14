#include "Physics.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <unordered_set>

std::vector<CastResult> Physics::cast_ray(World& world, sf::Vector3f& source, double angle) {
	std::vector<CastResult> hits;

	for (std::unique_ptr<Vertex>& vertex : world.verticies) {
		sf::Vector2f hit;
		if (hits_vertex(*vertex, hit, source, angle)) {
			double distance = sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
			hits.push_back(CastResult(sf::Vector2f(hit.x, hit.y), distance, vertex.get()));
		}
	}

	for (std::unique_ptr<Entity>& entity : world.entities) {
		sf::Vector2f hit;
		const std::vector<Vertex>& verticies = world.load_model(entity->model);
		for (const Vertex& vertex : verticies) {
			Vertex temp = vertex.translated(entity->location);
			Vertex* transformed = new Vertex(temp);
			if (hits_vertex(*transformed, hit, source, angle)) {
				double distance = sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
				hits.push_back(CastResult(sf::Vector2f(hit.x, hit.y), distance, transformed, entity.get()));
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
	for (std::unique_ptr<Entity>& entity_ptr : world.entities) {
		Entity& entity = *entity_ptr;
		entity.velocity += scale(entity.acceleration, dt);

		sf::Vector2f friction = scale(normalize(entity.velocity), world.friction * entity.mass * world.gravity * dt);
		if (mag(friction) > mag(entity.velocity)) {
			entity.velocity = sf::Vector2f();
		} else {
			entity.velocity -= friction;
		}

		std::vector<CastResult> potential_hits;
		std::unordered_set<Vertex*> whitelist;

		while ((potential_hits = cast_ray(world, entity.location, direction(entity.velocity))).size()) {
			bool valid = true;
			CastResult& result = potential_hits[0];
			int i = 0;
			while (whitelist.contains(result.vertex)) {
				i++;
				if (i >= potential_hits.size()) {
					valid = false;
					break;
				}
				result = potential_hits[i];
			}

			if (!valid) {
				break;
			}

			if (result.distance > mag(entity.velocity)) {
				break;
			}
			if (result.entity && (result.distance < entity.radius + result.entity->radius)) {
				sf::Vector2f hit = project(entity.velocity, sf::Vector2f(entity.location.x - result.entity->location.x, entity.location.y - result.entity->location.y));
				sf::Vector2f total_momentum = scale(hit, entity.mass) + scale(result.entity->velocity, result.entity->mass);
				sf::Vector2f new_velocity = scale(total_momentum, 1 / (entity.mass + result.entity->mass));
				entity.velocity += new_velocity - hit;
				result.entity->velocity = new_velocity;
				break;
			}
			else {
				sf::Vector2f wall_tangent = normalize(result.vertex->start - result.vertex->end);
				sf::Vector2f wall_normal = sf::Vector2f(-wall_tangent.y, wall_tangent.x);
				sf::Vector2f dist_vector = sf::Vector2f(result.point.x - entity.location.x, result.point.y - entity.location.y);
				sf::Vector2f projected_dist_vector = project(dist_vector, wall_normal);
				sf::Vector2f scaled_dist_vector = scale(normalize(projected_dist_vector), std::fmax(0, mag(projected_dist_vector) - entity.radius));

				sf::Vector2f tangent_velocity = project(entity.velocity, wall_tangent);
				sf::Vector2f normal_velocity = project(entity.velocity, wall_normal);

				double normal_distance = mag(scaled_dist_vector);
				
				if (mag(scale(normal_velocity, dt)) >= normal_distance) {
					if (entity.radius >= normal_distance) {
						scaled_dist_vector = scale(normalize(projected_dist_vector), mag(projected_dist_vector) - entity.radius);
					}
					entity.location.x += scaled_dist_vector.x;
					entity.location.y += scaled_dist_vector.y;
					normal_velocity = sf::Vector2f(0, 0);
				}
				
				entity.velocity = tangent_velocity + normal_velocity;
			}
			whitelist.insert(result.vertex);
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
	return scale_angle(std::atan2(vec.y, vec.x));
}

sf::Vector2f Physics::project(sf::Vector2f source, sf::Vector2f target) {
	double target_mag = mag(target);
	if (target_mag == 0) {
		return sf::Vector2f(0, 0);
	}
	return scale(normalize(target), dot(source, target) / target_mag);
}

double Physics::distance(sf::Vector2f a, sf::Vector2f b) {
	return mag(a - b);
}

double Physics::to_radians(double angle) {
	return M_PI * angle/180;
}

double Physics::to_degrees(double angle) {
	return 180 * angle/M_PI;
}

sf::Vector2f Physics::squash(sf::Vector3f input) {
	return sf::Vector2f(input.x, input.y);
}

sf::Vector2f Physics::mult(sf::Vector2f a, sf::Vector2f b) {
	return sf::Vector2f(a.x * b.x, a.y * b.y);
}
