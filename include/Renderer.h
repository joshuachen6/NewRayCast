#pragma once
#include "SFML/System/Vector3.hpp"
#include "Vertex.h"
#include "World.h"
#include <SFML/Graphics.hpp>

class Renderer {
private:
  sf::RenderWindow *window;
  sf::RenderTexture render_texture;
  sf::Sprite *get_column(sf::Texture *texture, const Vertex &vertex,
                         sf::Vector2f &collision, int cols);
  sf::Font font;
  sf::Shader noise_shader;

  void draw_minimap(World &world, sf::Vector3f &camera);

  sf::Text text_of(std::string text);

public:
  bool debug;
  bool show_minimap;
  Renderer(sf::RenderWindow &window);
  void update(World &world, sf::Vector3f &camera, double fov, double rays,
              double dt);
};
