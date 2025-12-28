#pragma once
#include <SFML/Audio.hpp>
#include <list>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class Audio {
private:
  std::unordered_map<std::string, sf::SoundBuffer> soundMap;
  std::list<sf::Sound> playingSound;
  std::list<sf::Music> playingMusic;

public:
  void update();
  void playSound(std::string sound);
  void playMusic(std::string music);

  static void initLua(lua_State *L);
};
