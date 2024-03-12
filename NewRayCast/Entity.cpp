#include "Entity.h"
#define _USE_MATH_DEFINES
#include <math.h>

std::vector<Vertex> Entity::to_vertcies() {
    std::vector<Vertex> vertcies;
    sf::Vector2f delta(std::cos(location.z),  std::sin(location.z));

    sf::Vector2f pdelta(std::cos(location.z + M_PI_2), std::sin(location.z + M_PI_2));

    sf::Vector2f pos(location.x, location.y);
    
    vertcies.push_back(Vertex(
        pos + delta * (size.x / 2),
        pos - delta * (size.x / 2),
        front
    ));
    for (std::pair<std::string, float>& plane: side) {
        vertcies.push_back(Vertex(
            pos + pdelta * (size.y / 2) + delta * plane.second,
            pos - pdelta * (size.y / 2) + delta * plane.second,
            plane.first
        ));
    }

    return vertcies;
}
