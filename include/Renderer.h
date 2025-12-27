#pragma once
#include "SFML/System/Vector3.hpp"
#include "Vertex.h"
#include "World.h"
#include <SFML/Graphics.hpp>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>
#include <unordered_map>

class Renderer {
private:
  sf::RenderWindow *window;
  sf::RenderTexture render_texture;
  sf::Sprite *get_column(sf::Texture *texture, const Vertex &vertex,
                         sf::Vector2f &collision, int cols);

  std::unordered_map<std::string, sf::Font> font_map;

  sf::Shader noise_shader;

  void draw_minimap(World &world, sf::Vector3f &camera);

  sf::Text text_of(std::string text, std::string font);

public:
  bool debug;
  bool show_minimap;
  Renderer(sf::RenderWindow &window);
  void update(World &world, sf::Vector3f &camera, double fov, double rays,
              double dt);

  sf::Font &getFont(std::string font);

  sf::Vector2f getSize();

  void drawRectangle(sf::Vector2f position, sf::Vector2f size, sf::Color color);
  void drawCircle(sf::Vector2f position, int radius, sf::Color color);
  void drawText(sf::Vector2f position, std::string font, std::string text,
                int size, sf::Color color);

  static void initLua(lua_State *L);
};
