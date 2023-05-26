#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include "TexturedVertex.h"


using namespace std;
using namespace sf;


class ResourceManager {
public:
	static vector<Texture*> textures;
	static vector<TexturedVertex> vertecies;
	static string current;
	static int vertexID;

	static void load(string file);
	static Sprite subTexture(TexturedVertex vertex, Vector2f collision ,int cols);
	static void addVertex(TexturedVertex vertex);
	static void addTexture(Texture* texture);
	static void destroy();
};

