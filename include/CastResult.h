#pragma once
#include "Entity.h"
#include "Vertex.h"
#include <SFML/Graphics.hpp>
#include <memory>

class CastResult {
private:
  std::shared_ptr<Vertex> temp_vertex;

public:
  Vertex *vertex;
  Entity *entity;
  sf::Vector2f point;
  double distance;

  CastResult(sf::Vector2f point, double distance, Vertex *vertex = nullptr,
             Entity *entity = nullptr);
  CastResult(const CastResult &other);
};
