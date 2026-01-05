#pragma once
#include "CastResult.h"
#include "Vertex.h"
#include "World.h"
#include <SFML/System.hpp>
#include <vector>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

const float METER = 100;

class Physics {
public:
  static std::vector<CastResult> cast_ray(World &world,
                                          const sf::Vector3f &source,
                                          float angle, float distance);
  static float scale_angle(float radians);
  static bool hits_vertex(Vertex &vertex, sf::Vector2f &point,
                          const sf::Vector3f &source, float slope,
                          sf::Vector2f lvector);
  static void apply_physics(World &world, float dt);

  static float dot(sf::Vector2f a, sf::Vector2f b);
  static sf::Vector2f scale(sf::Vector2f vec, float scalar);
  static sf::Vector2f normalize(sf::Vector2f vec);
  static float mag(sf::Vector2f vec);
  static float direction(sf::Vector2f vec);
  static sf::Vector2f project(sf::Vector2f source, sf::Vector2f target);
  static float distance(sf::Vector2f a, sf::Vector2f b);

  static float to_radians(float angle);
  static float to_degrees(float angle);

  static sf::Vector2f squash(sf::Vector3f input);
  static sf::Vector2f ham(sf::Vector2f a, sf::Vector2f b);
  static void initLua(lua_State *L);
};
