// Zmiany w pliku level_manager.h

#pragma once
#include "../map/map.h"
#include "../player/player.h"
#include <string>
#include <vector>

class LevelManager {
public:
  // Zmodyfikowany konstruktor przyjmujący dwie tekstury bloków
  LevelManager(SDL_Renderer *renderer, SDL_Texture *grassBlockTexture,
               SDL_Texture *plainBlockTexture);
  ~LevelManager();

  bool initialize(const std::string &levelDirPath);

  bool loadLevel(int levelNum, Player &player, Map &map);

  bool goToNextLevel(Player &player, Map &map);

  bool isGameCompleted() const;

  int getCurrentLevel() const;

  void setExitTexture(SDL_Texture *exitTexture);

  SDL_Texture *getExitTexture() const;

private:
  SDL_Renderer *renderer;
  SDL_Texture *grassBlockTexture; // Tekstura bloku z trawą
  SDL_Texture *plainBlockTexture; // Tekstura bloku bez trawy
  SDL_Texture *exitTexture;

  int currentLevel;
  int maxLevels;
  std::string levelPath;

  bool levelFileExists(int levelNum) const;
};
