#include "TexturedVertex.h"

TexturedVertex::TexturedVertex(Vector2f start, Vector2f end, int textureID) {
	this->start = start;
	this->end = end;
	this->textureID = textureID;
}

double TexturedVertex::length() {
	return sqrt(pow(start.x - end.x, 2) + pow(start.y - end.y, 2));
}
