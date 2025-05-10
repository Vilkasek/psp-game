#pragma once
#include "../map/map.h"
#include "../player/player.h"
#include <string>
#include <vector>

class LevelManager {
public:
  LevelManager(SDL_Renderer *renderer, SDL_Texture *blockTexture);
  ~LevelManager();

  // Inicjalizacja menedżera poziomów
  bool initialize(const std::string &levelDirPath);

  // Ładowanie konkretnego poziomu
  bool loadLevel(int levelNum, Player &player, Map &map);

  // Przejście do następnego poziomu
  bool goToNextLevel(Player &player, Map &map);

  // Sprawdzenie, czy gracz ukończył wszystkie poziomy
  bool isGameCompleted() const;

  // Pobranie aktualnego numeru poziomu
  int getCurrentLevel() const;

  // Dodanie tekstury wyjścia
  void setExitTexture(SDL_Texture *exitTexture);

  // Pobranie tekstury wyjścia
  SDL_Texture *getExitTexture() const;

private:
  SDL_Renderer *renderer;
  SDL_Texture *blockTexture;
  SDL_Texture *exitTexture; // Tekstura dla wyjścia

  int currentLevel;
  int maxLevels;
  std::string levelPath;

  // Sprawdzenie, czy plik poziomu istnieje
  bool levelFileExists(int levelNum) const;
};
