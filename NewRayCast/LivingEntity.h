#pragma once
#include "Entity.h"
#include "Behavior.h"

class Behavior;

class LivingEntity: public Entity {
public:
	Behavior* behavior;
	double health;
};

