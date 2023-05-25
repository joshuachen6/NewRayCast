#pragma once
#include <SFML/Graphics.hpp>
#include <vector>


using namespace sf;
using namespace std;


class TexturedVertex
{
public:
	Vector2f start;
	Vector2f end;
	int textureID;

	TexturedVertex(Vector2f start, Vector2f end, int textureID);
	double length();
};

