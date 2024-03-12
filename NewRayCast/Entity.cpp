#include "Entity.h"
#define _USE_MATH_DEFINES
#include <math.h>

std::vector<Vertex> Entity::to_vertcies() {
    std::vector<Vertex> vertcies;
    double dx = std::cos(location.z) * radius;
    double dy = std::sin(location.z) * radius;

    double pdx = std::cos(location.z + M_PI_2) * radius;
    double pdy = std::sin(location.z + M_PI_2) * radius;
    
    vertcies.push_back(Vertex(sf::Vector2f(location.x + pdx, location.y + pdy), sf::Vector2f(location.x - pdx, location.y - pdy), front));
    vertcies.push_back(Vertex(sf::Vector2f(location.x + dx, location.y + dy), sf::Vector2f(location.x - dx, location.y - dy), side));

    return vertcies;
}
