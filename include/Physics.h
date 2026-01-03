#pragma once
#include "CastResult.h"
#include "Vertex.h"
#include "World.h"
#include <SFML/System.hpp>
#include <vector>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

const double METER = 100;

class Physics {
public:
  static std::vector<CastResult> cast_ray(World &world, const sf::Vector3f &source, double angle);
  static double scale_angle(double radians);
  static bool hits_vertex(Vertex &vertex, sf::Vector2f &point, const sf::Vector3f &source, double slope, sf::Vector2f lvector);
  static void apply_physics(World &world, double dt);

  static double dot(sf::Vector2f a, sf::Vector2f b);
  static sf::Vector2f scale(sf::Vector2f vec, double scalar);
  static sf::Vector2f normalize(sf::Vector2f vec);
  static double mag(sf::Vector2f vec);
  static double direction(sf::Vector2f vec);
  static sf::Vector2f project(sf::Vector2f source, sf::Vector2f target);
  static double distance(sf::Vector2f a, sf::Vector2f b);

  static double to_radians(double angle);
  static double to_degrees(double angle);

  static sf::Vector2f squash(sf::Vector3f input);
  static sf::Vector2f ham(sf::Vector2f a, sf::Vector2f b);
  static void initLua(lua_State *L);
};
