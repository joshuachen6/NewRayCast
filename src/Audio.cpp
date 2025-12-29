#include "Audio.h"
#include "Physics.h"
#include "SFML/Audio/Listener.hpp"
#include "SFML/Audio/Sound.hpp"
#include <spdlog/spdlog.h>

void Audio::update() {
  playingSound.remove_if([](const sf::Sound &sound) {
    return sound.getStatus() == sf::Sound::Stopped;
  });
}

sf::Sound *Audio::loadSound(std::string sound) {

  if (not soundMap.contains(sound)) {
    if (not soundMap[sound].loadFromFile(sound)) {
      spdlog::error("Failed to load sound {}", sound);
      return nullptr;
    }
  }
  return &playingSound.emplace_back(soundMap[sound]);
}

void Audio::playSound(std::string sound) {
  sf::Sound *soundObject = loadSound(sound);
  if (soundObject) {
    soundObject->setRelativeToListener(true);
    soundObject->play();
  }
}

void Audio::playSoundAt(std::string sound, sf::Vector2f position) {
  sf::Sound *soundObject = loadSound(sound);
  if (soundObject) {
    soundObject->setRelativeToListener(false);
    soundObject->setPosition({position.x, 0, position.y});
    soundObject->setMinDistance(100.f);
    soundObject->setAttenuation(1.0f);
    soundObject->play();
  }
}

void Audio::playMusic(std::string music) {
  if (not playingMusic.openFromFile(music)) {
    spdlog::error("Failed to load music {}", music);
    return;
  };
  playingMusic.play();
}

void Audio::setListener(const sf::Vector3f &position) {
  sf::Listener::setPosition({position.x, 0, position.y});
  sf::Listener::setDirection(cos(position.z), 0, sin(position.z));
}

void Audio::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<Audio>("Audio")
      .addFunction("play_sound", &Audio::playSound)
      .addFunction("play_music", &Audio::playMusic)
      .addFunction("play_sound_at", &Audio::playSoundAt)
      .endClass();
}
