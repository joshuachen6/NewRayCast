#include "SFML/Window/Keyboard.hpp"

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

namespace Controls {
bool keyPressed(int key);

void initLua(lua_State *L);
} // namespace Controls
