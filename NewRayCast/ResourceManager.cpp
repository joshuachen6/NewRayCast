#include "ResourceManager.h"
#include <fstream>
#include <string>
#include <iostream>


vector<Texture*> ResourceManager::textures;
vector<TexturedVertex> ResourceManager::vertecies;
int ResourceManager::vertexID = 0;


void ResourceManager::load(string file) {
	ifstream stream(file);
	string line;
	bool textureFiles = true;
	if (stream.is_open()) {
		while (getline(stream, line)) {
			if (textureFiles) {
				if (line == "ENDFILES") {
					textureFiles = false;
					continue;
				}

				Texture* texture = new Texture();
				texture->loadFromFile(line);
				textures.push_back(texture);
			} else {
				Vector2f start, end;
				int textureID = 0;

				int lastIndex = 0;
				int itemIndex = 0;
				for (int i = 0; i < line.size(); i++) {
					if (line[i] == ',') {
						double value = stod(line.substr(lastIndex, i - lastIndex));
						if (itemIndex == 0) {
							start.x = value;
						} else if (itemIndex == 1) {
							start.y = value;
						} else if (itemIndex == 2) {
							end.x = value;
						} else if (itemIndex == 3) {
							end.y = value;
							textureID = stod(line.substr(i + 1, line.size() - i - 1));;
							break;
						}
						itemIndex++;
						lastIndex = i + 1;
					}
				}
				addVertex(TexturedVertex(start, end, textureID));
			}
		}
	}
}

Sprite ResourceManager::subTexture(TexturedVertex vertex, Vector2f collision, int cols) {
	Texture* texture = textures[vertex.textureID];
	Sprite sprite(*texture);
	double scale = texture->getSize().x / vertex.length();
	double left = sqrt(pow(collision.x - vertex.start.x, 2) + pow(collision.y - vertex.start.y, 2));
	sprite.setTextureRect(IntRect(left*scale, 0, cols, texture->getSize().y));
	return sprite;
}

void ResourceManager::addVertex(TexturedVertex vertex) {
	vertex.ID = vertexID;
	vertexID++;
	ResourceManager::vertecies.push_back(vertex);
}

void ResourceManager::addTexture(Texture* texture) {
	ResourceManager::textures.push_back(texture);
}

void ResourceManager::removeVertex(int id) {
	for (int i = 0; i < vertecies.size(); i++) {
		if (vertecies[i].ID == id) {
			vertecies.erase(vertecies.begin() + i, vertecies.begin() + i + 1);
		}
	}
}

void ResourceManager::destroy() {
	for (Texture* texture : ResourceManager::textures) {
		delete texture;
	}
}
