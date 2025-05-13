#include "map.h"
#include <algorithm>
#include <fstream>

Map::Map(SDL_Renderer *renderer, SDL_Texture *blockTexture)
    : renderer(renderer), texture(blockTexture), exitTexture(nullptr),
      exitExists(false), worldWidth(0), worldHeight(0) {}

void Map::setExitTexture(SDL_Texture *texture) { exitTexture = texture; }
float Map::getPlayerMapX() { return playerMapX; }

float Map::getPlayerMapY() { return playerMapY; }

const std::vector<Block> &Map::getBlocks() const { return blocks; }

void Map::loadFromFile(const char *filename) {
  blocks.clear();
  exitExists = false;
  worldWidth = 0;
  worldHeight = 0;

  std::ifstream file(filename);

  if (!file.is_open()) {
    return;
  }

  std::string line;
  int row = 0;
  int maxCol = 0;

  while (std::getline(file, line)) {
    maxCol = std::max(maxCol, static_cast<int>(line.length()));
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

  // Ustaw wymiary świata gry na podstawie wczytanej mapy
  worldWidth = maxCol * tileSize;
  worldHeight = row * tileSize;

  // Zapewnij minimalny rozmiar świata
  worldWidth = std::max(worldWidth, 480);
  worldHeight = std::max(worldHeight, 272);
}

void Map::render(SDL_Renderer *renderer, const Camera *camera) {
  for (auto &block : blocks) {
    if (camera) {
      // Renderuj blok z uwzględnieniem kamery
      SDL_Rect worldRect = block.getRect();
      SDL_Rect screenRect = camera->worldToScreen(worldRect);

      // Renderuj tylko jeśli widoczny na ekranie
      if (screenRect.x + screenRect.w >= 0 && screenRect.x <= 480 &&
          screenRect.y + screenRect.h >= 0 && screenRect.y <= 272) {

        // Użyj tekstury bloku, ale z przesunięciem kamery
        if (block.getRect().w == tileSize && block.getRect().h == tileSize) {
          SDL_RenderCopy(renderer, texture, NULL, &screenRect);
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
          SDL_RenderFillRect(renderer, &screenRect);
        }
      }
    } else {
      // Stary sposób bez kamery
      block.render(renderer);
    }
  }

  if (exitExists) {
    SDL_Rect exitRectToRender = exitRect;

    if (camera) {
      exitRectToRender = camera->worldToScreen(exitRect);
    }

    if (exitTexture) {
      SDL_RenderCopy(renderer, exitTexture, NULL, &exitRectToRender);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderFillRect(renderer, &exitRectToRender);

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &exitRectToRender);
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

int Map::getWorldWidth() const { return worldWidth; }

int Map::getWorldHeight() const { return worldHeight; }
