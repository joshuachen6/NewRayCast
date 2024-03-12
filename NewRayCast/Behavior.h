#pragma once
#include "LivingEntity.h"
#include "World.h"

class LivingEntity;

class Behavior {
public:
	virtual void update(LivingEntity& entity, World& world) = 0;
};

