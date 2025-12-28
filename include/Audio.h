#pragma once
#include <SFML/Audio.hpp>
#include <list>

#include <lua.hpp>

#include <LuaBridge/LuaBridge.h>

class Audio {
private:
  std::unordered_map<std::string, sf::SoundBuffer> soundMap;
  std::list<sf::Sound> playingSound;
  sf::Music playingMusic;

public:
  void update();
  sf::Sound *loadSound(std::string sound);
  void playSound(std::string sound);
  void playSoundAt(std::string sound, sf::Vector2f position);
  void playMusic(std::string music);
  void setListener(const sf::Vector3f &position);

  static void initLua(lua_State *L);
};
