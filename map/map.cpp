#include "map.h"
#include <fstream>

Map::Map(SDL_Renderer *renderer, SDL_Texture *blockTexture)
    : renderer(renderer), texture(blockTexture), exitTexture(nullptr),
      exitExists(false) {}

void Map::setExitTexture(SDL_Texture *texture) { exitTexture = texture; }
float Map::getPlayerMapX() { return playerMapX; }

float Map::getPlayerMapY() { return playerMapY; }

const std::vector<Block> &Map::getBlocks() const { return blocks; }

void Map::loadFromFile(const char *filename) {
  blocks.clear();
  exitExists = false;

  std::ifstream file(filename);

  if (!file.is_open()) {
    return;
  }

  std::string line;
  int row = 0;

  while (std::getline(file, line)) {
    for (int col = 0; col < line.length(); ++col) {
      if (line[col] == '#') {
        blocks.emplace_back(col * tileSize, row * tileSize, renderer, texture);
      } else if (line[col] == 'p') {
        playerMapX = col * tileSize;
        playerMapY = row * tileSize;
      } else if (line[col] == 'E') {
        exitExists = true;
        exitRect = {col * tileSize, row * tileSize, tileSize, tileSize};
      }
    }
    row++;
  }
}

void Map::render(SDL_Renderer *renderer) {
  for (auto &block : blocks) {
    block.render(renderer);
  }

  if (exitExists) {
    if (exitTexture) {
      SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderFillRect(renderer, &exitRect);

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &exitRect);
    }
  }
}
bool Map::hasExit() const { return exitExists; }

SDL_Rect Map::getExitRect() const { return exitRect; }

void Map::clearExit() { exitExists = false; }

bool Map::checkExitCollision(SDL_Rect playerRect) const {
  if (!exitExists) {
    return false;
  }
  return SDL_HasIntersection(&playerRect, &exitRect);
}
