#include "Audio.h"
#include <spdlog/spdlog.h>

void Audio::update() {
  playingSound.remove_if([](const sf::Sound &sound) {
    return sound.getStatus() == sf::Sound::Stopped;
  });
  playingMusic.remove_if([](const sf::Music &music) {
    return music.getStatus() == sf::Music::Stopped;
  });
}

void Audio::playSound(std::string sound) {
  if (not soundMap.contains(sound)) {
    if (not soundMap[sound].loadFromFile(sound)) {
      spdlog::error("Failed to load sound {}", sound);
      return;
    }
  }
  sf::Sound &soundObject = playingSound.emplace_back(soundMap[sound]);
  soundObject.play();
}

void Audio::playMusic(std::string music) {
  sf::Music &musicObject = playingMusic.emplace_back();
  if (not musicObject.openFromFile(music)) {
    spdlog::error("Failed to load music {}", music);
    playingMusic.pop_back();
    return;
  };
  musicObject.play();
}

void Audio::initLua(lua_State *L) {
  luabridge::getGlobalNamespace(L)
      .beginClass<Audio>("Audio")
      .addFunction("play_sound", &Audio::playSound)
      .addFunction("play_music", &Audio::playMusic)
      .endClass();
}
