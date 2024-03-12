#include "Vertex.h"

Vertex::Vertex(sf::Vector2f start, sf::Vector2f end, std::string texture) {
    this->start = start;
    this->end = end;
    this->texture = texture;
}

double Vertex::length() {
    return std::sqrt(std::pow(start.x - end.x, 2) + std::powf(start.y - end.y, 2));
}
