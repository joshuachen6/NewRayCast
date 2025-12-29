#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class Controls {
private:
  std::unordered_map<int, bool> pressed;

public:
  bool keyPressed(int key);
  bool keyJustPressed(int key);
  void update();

  static void initLua(lua_State *L);
};
