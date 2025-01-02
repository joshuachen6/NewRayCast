#include "Vertex.h"
#include <cmath>

Vertex::Vertex(sf::Vector2f start, sf::Vector2f end, double height, double z,
               std::string texture) {
  this->start = start;
  this->end = end;
  this->height = height;
  this->z = z;
  this->texture = texture;
}

Vertex::Vertex(const Vertex &other) {
  start = other.start;
  end = other.end;
  height = other.height;
  z = other.z;
  texture = other.texture;
}

Vertex Vertex::translated(sf::Vector2f translate, double rotation) const {
  return Vertex(
      sf::Vector2f(start.x * std::cos(rotation) - start.y * std::sin(rotation),
                   start.x * std::sin(rotation) +
                       start.y * std::cos(rotation)) +
          translate,
      sf::Vector2f(end.x * std::cos(rotation) - end.y * std::sin(rotation),
                   end.x * std::sin(rotation) + end.y * std::cos(rotation)) +
          translate,
      height, z, texture);
}

Vertex Vertex::translated(sf::Vector3f translate) const {
  return translated(sf::Vector2f(translate.x, translate.y), translate.z);
}

double Vertex::length() const {
  return std::sqrt(std::pow(start.x - end.x, 2) + std::pow(start.y - end.y, 2));
}
