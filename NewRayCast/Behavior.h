#pragma once
#include "Entity.h"
#include "World.h"
#include <unordered_map>
#include <sol/sol.hpp>

class Behavior {
public:
	std::unordered_map<std::string, sol::object> data;
};

