#include "level_manager.h"
#include <fstream>
#include <iostream>

LevelManager::LevelManager(SDL_Renderer *renderer, SDL_Texture *blockTexture)
    : renderer(renderer), blockTexture(blockTexture), exitTexture(nullptr),
      currentLevel(1), maxLevels(0), levelPath("maps/") {}

LevelManager::~LevelManager() {
  // Nie zwalniamy tekstur, ponieważ są one własnością wywołującego
}

bool LevelManager::initialize(const std::string &levelDirPath) {
  levelPath = levelDirPath;
  if (levelPath.back() != '/') {
    levelPath += '/';
  }

  // Znajdź maksymalną liczbę poziomów
  maxLevels = 0;
  int level = 1;
  while (levelFileExists(level)) {
    maxLevels = level;
    level++;
  }

  if (maxLevels == 0) {
    std::cerr << "No level files found in directory: " << levelPath
              << std::endl;
    return false;
  }

  std::cout << "Found " << maxLevels << " levels" << std::endl;
  return true;
}

bool LevelManager::levelFileExists(int levelNum) const {
  std::string filename =
      levelPath + "level" + std::to_string(levelNum) + ".txt";
  std::ifstream file(filename);
  return file.good();
}

bool LevelManager::loadLevel(int levelNum, Player &player, Map &map) {
  if (levelNum < 1 || levelNum > maxLevels) {
    std::cerr << "Invalid level number: " << levelNum << std::endl;
    return false;
  }

  std::string filename =
      levelPath + "level" + std::to_string(levelNum) + ".txt";
  map.loadFromFile(filename.c_str());

  player.setPosition(static_cast<int>(map.getPlayerMapX()),
                     static_cast<int>(map.getPlayerMapY()));

  currentLevel = levelNum;
  std::cout << "Loaded level " << currentLevel << std::endl;
  return true;
}

bool LevelManager::goToNextLevel(Player &player, Map &map) {
  int nextLevel = currentLevel + 1;
  if (nextLevel > maxLevels) {
    std::cout << "Congratulations! You completed all levels!" << std::endl;
    return false;
  }

  return loadLevel(nextLevel, player, map);
}

bool LevelManager::isGameCompleted() const { return currentLevel > maxLevels; }

int LevelManager::getCurrentLevel() const { return currentLevel; }

void LevelManager::setExitTexture(SDL_Texture *exitTexture) {
  this->exitTexture = exitTexture;
}

SDL_Texture *LevelManager::getExitTexture() const { return exitTexture; }
