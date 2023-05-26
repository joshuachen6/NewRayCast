#pragma once
#include <SFML/Graphics.hpp>
#include "TexturedVertex.h"


const double PI = 3.14159265;

typedef struct CastResult {
	TexturedVertex vertex;
	double cx, cy;
	double distance;

	CastResult(TexturedVertex vertex, double cx, double cy, double distance);
};

vector<CastResult> castRay(double angle, double FOV, Vector3f player);


double scaleAngle(double angle);