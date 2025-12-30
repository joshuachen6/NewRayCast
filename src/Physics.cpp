#include "Physics.h"
#include "SFML/System/Vector3.hpp"
#include <LuaBridge/LuaBridge.h>
#include <lua.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include <set>
#include <spdlog/spdlog.h>

std::vector<CastResult>
Physics::cast_ray(World &world, const sf::Vector3f &source, double angle) {
  std::vector<CastResult> hits;

  for (int i = 0; i < world.vertices.size(); ++i) {
    std::unique_ptr<Vertex> &vertex = world.vertices[i];
    sf::Vector2f hit;
    if (hits_vertex(*vertex, hit, source, angle)) {
      double distance =
          sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
      hits.push_back(
          CastResult(i, nullptr, sf::Vector2f(hit.x, hit.y), {}, distance));
    }
  }

  for (std::unique_ptr<Entity> &entity : world.entities) {
    if (entity->model.empty())
      continue;
    sf::Vector2f hit;
    const std::vector<Vertex> &vertices = world.load_model(entity->model);
    for (int i = 0; i < vertices.size(); ++i) {
      const Vertex &vertex = vertices[i];
      Vertex temp = vertex.translated(entity->location);
      if (hits_vertex(temp, hit, source, angle)) {
        double distance =
            sqrt(pow(hit.x - source.x, 2) + pow(hit.y - source.y, 2));
        hits.push_back(CastResult(i, entity.get(), sf::Vector2f(hit.x, hit.y),
                                  entity->location, distance));
      }
    }
  }

  sort(hits.begin(), hits.end(),
       [](CastResult a, CastResult b) { return a.distance < b.distance; });

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

bool Physics::hits_vertex(Vertex &vertex, sf::Vector2f &point,
                          const sf::Vector3f &source, double angle) {
  double slope = tan(angle);
  double b = source.y - source.x * slope;
  double cx, cy;
  if (vertex.start.x != vertex.end.x) {
    double vslope =
        (vertex.end.y - vertex.start.y) / (vertex.end.x - vertex.start.x);
    bool collides = (slope - vslope) != 0;
    if (!collides) {
      return false;
    }
    double vb = -vslope * vertex.start.x + vertex.start.y;
    cx = (vb - b) / (slope - vslope);
    cy = slope * cx + b;
  } else {
    cy = slope * vertex.start.x + b;
    cx = vertex.start.x;
  }

  bool yrange = (cy - std::min(vertex.start.y, vertex.end.y)) > -0.5 &&
                (cy - std::max(vertex.start.y, vertex.end.y)) < 1;
  bool xrange = (cx - std::min(vertex.start.x, vertex.end.x)) > -0.5 &&
                (cx - std::max(vertex.start.x, vertex.end.x)) < 1;

  if (!(yrange && xrange)) {
    return false;
  }

  if (std::abs(scale_angle(
          (atan2((cy - source.y), (cx - source.x)) - source.z))) >= M_PI_2) {
    return false;
  }

  point.x = cx;
  point.y = cy;

  return true;
}

void Physics::apply_physics(World &world, double dt) {
  for (int entity_index = 0; entity_index < world.entities.size();
       entity_index++) {
    Entity &entity = *world.entities[entity_index];
    entity.velocity += scale(entity.acceleration, dt);

    sf::Vector2f friction =
        scale(normalize(entity.velocity),
              world.friction * entity.mass * world.gravity * dt);
    if (mag(friction) > mag(entity.velocity)) {
      entity.velocity = sf::Vector2f();
    } else {
      entity.velocity -= friction;
    }

    // momentum
    for (int other_index = entity_index + 1;
         other_index < world.entities.size(); other_index++) {
      Entity &other = *world.entities[other_index];
      sf::Vector2f distance_vector =
          squash(other.location) - squash(entity.location);
      double distance = mag(distance_vector);
      if (distance < entity.radius + other.radius) {
        // Collision event
        if (entity.onCollide.isFunction())
          entity.onCollide(&entity, &other);
        if (other.onCollide.isFunction())
          other.onCollide(&other, &entity);

        if (not entity.has_collision or not other.has_collision) {
          continue;
        }

        sf::Vector2f normalized_dist = normalize(distance_vector);

        if (dot(distance_vector, entity.velocity - other.velocity) > 0) {
          sf::Vector2f entity_normal =
              project(entity.velocity, normalized_dist);
          sf::Vector2f other_normal = project(other.velocity, normalized_dist);
          double new_velocity = (mag(entity_normal) * entity.mass +
                                 mag(other_normal) * other.mass) /
                                (entity.mass + other.mass);
          sf::Vector2f new_normal =
              scale(normalize(distance_vector), new_velocity);

          entity.velocity -= entity_normal;
          other.velocity -= other_normal;

          if (!other.is_static) {
            entity.velocity += new_normal;
            other.velocity += new_normal;
          }
        }

        sf::Vector2f offset =
            scale(normalized_dist, distance - (entity.radius + other.radius));

        if (!other.is_static) {
          entity.location.x += offset.x / 2;
          entity.location.y += offset.y / 2;
          other.location.x -= offset.x / 2;
          other.location.y -= offset.y / 2;
        } else {
          entity.location.x += offset.x;
          entity.location.y += offset.y;
        }
      }
    }

    std::vector<CastResult> potential_hits;
    std::set<std::pair<int, Entity *>> whitelist;

    double dir = direction(entity.velocity);
    sf::Vector3f source(entity.location.x, entity.location.y, dir);

    while ((potential_hits = cast_ray(world, source, dir)).size()) {
      bool valid = true;
      CastResult &result = potential_hits[0];
      int i = 0;
      while (whitelist.contains({result.index, result.owner}) or
             result.owner == &entity or
             (result.owner and not result.owner->has_collision)) {
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

      if (result.distance > mag(entity.velocity) * dt) {
        break;
      }

      if (not result.owner and entity.onCollide) {
        entity.onCollide(&entity);
      }

      if (not entity.has_collision) {
        break;
      }

      const Vertex &temp =
          result.owner ? world.load_model(result.owner->model)[result.index]
                       : *world.vertices[result.index];

      Vertex vertex =
          result.owner ? temp.translated(result.ownerLocation) : temp;

      sf::Vector2f wall_tangent = normalize(vertex.start - vertex.end);
      sf::Vector2f wall_normal = sf::Vector2f(-wall_tangent.y, wall_tangent.x);
      sf::Vector2f dist_vector =
          sf::Vector2f(result.point.x - entity.location.x,
                       result.point.y - entity.location.y);
      sf::Vector2f projected_dist_vector = project(dist_vector, wall_normal);
      sf::Vector2f scaled_dist_vector =
          scale(normalize(projected_dist_vector),
                std::fmax(0, mag(projected_dist_vector) - entity.radius));

      sf::Vector2f tangent_velocity = project(entity.velocity, wall_tangent);
      sf::Vector2f normal_velocity = project(entity.velocity, wall_normal);

      double normal_distance = mag(scaled_dist_vector);

      if (mag(scale(normal_velocity, dt)) >= normal_distance) {
        if (entity.radius >= normal_distance) {
          scaled_dist_vector =
              scale(normalize(projected_dist_vector),
                    mag(projected_dist_vector) - entity.radius);
        }
        entity.location.x += scaled_dist_vector.x;
        entity.location.y += scaled_dist_vector.y;
        normal_velocity = sf::Vector2f(0, 0);
      }

      entity.velocity = tangent_velocity + normal_velocity;
      whitelist.insert({result.index, result.owner});
    }
    entity.location = sf::Vector3f(entity.location.x + entity.velocity.x * dt,
                                   entity.location.y + entity.velocity.y * dt,
                                   entity.location.z);

    dir = direction(entity.velocity);
    source = sf::Vector3f(entity.location.x, entity.location.y, dir);
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
  return sf::Vector2f(vec.x / magnitude, vec.y / magnitude);
}

double Physics::mag(sf::Vector2f vec) {
  return std::sqrt(vec.x * vec.x + vec.y * vec.y);
}

double Physics::direction(sf::Vector2f vec) { return std::atan2(vec.y, vec.x); }

sf::Vector2f Physics::project(sf::Vector2f source, sf::Vector2f target) {
  double target_mag = mag(target);
  if (target_mag == 0) {
    return sf::Vector2f(0, 0);
  }
  return scale(normalize(target), dot(source, target) / target_mag);
}

double Physics::distance(sf::Vector2f a, sf::Vector2f b) { return mag(a - b); }

double Physics::to_radians(double angle) { return M_PI * angle / 180; }

double Physics::to_degrees(double angle) { return 180 * angle / M_PI; }

sf::Vector2f Physics::squash(sf::Vector3f input) {
  return sf::Vector2f(input.x, input.y);
}

sf::Vector2f Physics::ham(sf::Vector2f a, sf::Vector2f b) {
  return sf::Vector2f(a.x * b.x, a.y * b.y);
}

// Functions for Lua
/// Addition
sf::Vector3f vec3_add(const sf::Vector3f &a, const sf::Vector3f &b) {
  return a + b;
}
// Subtraction
sf::Vector3f vec3_sub(const sf::Vector3f &a, const sf::Vector3f &b) {
  return a - b;
}
// Scalar Multiplication (Vector * float)
sf::Vector3f vec3_mul(const sf::Vector3f &a, float s) { return a * s; }
// Unary Minus (Negation)
sf::Vector3f vec3_neg(const sf::Vector3f &a) { return -a; }
/// Addition
sf::Vector2f vec2_add(const sf::Vector2f &a, const sf::Vector2f &b) {
  return a + b;
}
// Subtraction
sf::Vector2f vec2_sub(const sf::Vector2f &a, const sf::Vector2f &b) {
  return a - b;
}
// Scalar Multiplication (Vector * float)
sf::Vector2f vec2_mul(const sf::Vector2f &a, float s) { return a * s; }
// Unary Minus (Negation)
sf::Vector2f vec2_neg(const sf::Vector2f &a) { return -a; }

void Physics::initLua(lua_State *L) {

  luabridge::getGlobalNamespace(L)
      // Register sf::Vector3f
      .beginClass<sf::Vector3f>("Vector3")
      .addConstructor<void (*)(float, float, float)>()
      .addProperty("x", &sf::Vector3f::x)
      .addProperty("y", &sf::Vector3f::y)
      .addProperty("z", &sf::Vector3f::z)
      .addFunction("__add", vec3_add)
      .addFunction("__sub", vec3_sub)
      .addFunction("__mul", vec3_mul)
      .addFunction("__unm", vec3_neg)
      .endClass()
      // Register sf::Vector2f
      .beginClass<sf::Vector2f>("Vector2")
      .addConstructor<void (*)(float, float)>()
      .addProperty("x", &sf::Vector2f::x)
      .addProperty("y", &sf::Vector2f::y)
      .addFunction("__add", vec2_add)
      .addFunction("__sub", vec2_sub)
      .addFunction("__mul", vec2_mul)
      .addFunction("__unm", vec2_neg)
      .endClass()
      // Place in namespace
      .beginNamespace("physics")
      // Register helper functions
      .addFunction("scale_angle", scale_angle)
      .addFunction("dot", dot)
      .addFunction("scale", scale)
      .addFunction("normalize", normalize)
      .addFunction("mag", mag)
      .addFunction("direction", direction)
      .addFunction("project", project)
      .addFunction("distance", distance)
      .addFunction("to_radians", to_radians)
      .addFunction("to_degrees", to_degrees)
      .addFunction("squash", squash)
      .addFunction("ham", ham)
      .endNamespace();
}
