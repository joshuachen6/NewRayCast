#pragma once
#include "Entity.h"
#include <SFML/Graphics.hpp>

struct CastResult {
  int index;
  Entity *owner;
  sf::Vector2f point;
  sf::Vector3f ownerLocation;
  double distance;
};
