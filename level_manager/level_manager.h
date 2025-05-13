#pragma once
#include "../map/map.h"
#include "../player/player.h"
#include <string>
#include <vector>

class LevelManager {
public:
  LevelManager(SDL_Renderer *renderer, SDL_Texture *blockTexture);
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
  SDL_Texture *blockTexture;
  SDL_Texture *exitTexture;

  int currentLevel;
  int maxLevels;
  std::string levelPath;

  bool levelFileExists(int levelNum) const;
};
