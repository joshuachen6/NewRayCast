#include "Entity.h"
#include <boost/algorithm/string.hpp>

Entity::Entity(std::unordered_map<std::string, std::string>& data, sf::Vector3f location) {
	model = data["model"];
	script = data["script"];
	this->location = location;
	mass = std::stod(data["mass"]);
	radius = std::stod(data["radius"]);
	std::string is_static = data["is_static"];
	boost::to_lower(is_static);
	this->is_static = is_static == "true";
}
