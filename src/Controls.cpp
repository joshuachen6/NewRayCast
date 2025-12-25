#include "Controls.h"
#include "SFML/Window/Keyboard.hpp"

bool Controls::keyPressed(int key) {
  return sf::Keyboard::isKeyPressed((sf::Keyboard::Key)key);
}

void Controls::initLua(lua_State *L) {
  static int W = sf::Keyboard::Key::W;
  static int A = sf::Keyboard::Key::A;
  static int S = sf::Keyboard::Key::S;
  static int D = sf::Keyboard::Key::D;

  luabridge::getGlobalNamespace(L)
      // Add the function
      .addFunction("key_pressed", keyPressed)
      // Add the keys
      .beginNamespace("Key")
      .addProperty("W", &W, false)
      .addProperty("A", &A, false)
      .addProperty("S", &S, false)
      .addProperty("D", &D, false)
      .endNamespace();
}
