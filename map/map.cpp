#include "map.h"
#include <algorithm>
#include <fstream>

Map::Map(SDL_Renderer *renderer, SDL_Texture *grassBlockTexture,
         SDL_Texture *plainBlockTexture)
    : renderer(renderer), grassBlockTexture(grassBlockTexture),
      plainBlockTexture(plainBlockTexture), exitTexture(nullptr),
      exitExists(false), worldWidth(0), worldHeight(0) {}

void Map::setExitTexture(SDL_Texture *texture) { exitTexture = texture; }
float Map::getPlayerMapX() { return playerMapX; }

float Map::getPlayerMapY() { return playerMapY; }

const std::vector<Block> &Map::getBlocks() const { return blocks; }

bool Map::hasBlockAbove(int x, int y) const {
  // Sprawdź, czy istnieje blok na pozycji (x, y-1)
  for (const auto &block : blocks) {
    SDL_Rect rect = block.getRect();
    if (rect.x == x && rect.y == y - tileSize) {
      return true;
    }
  }
  return false;
}

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

  // Pierwsza pętla - wczytaj mapę do pamięci
  std::vector<std::string> mapData;
  while (std::getline(file, line)) {
    mapData.push_back(line);
    maxCol = std::max(maxCol, static_cast<int>(line.length()));
    row++;
  }

  // Druga pętla - analizuj mapę i twórz bloki
  for (int y = 0; y < mapData.size(); y++) {
    for (int x = 0; x < mapData[y].length(); x++) {
      if (mapData[y][x] == '#') {
        // Sprawdź, czy nad blokiem jest inny blok
        bool hasAbove = false;
        if (y > 0 && y < mapData.size() && x < mapData[y - 1].length()) {
          hasAbove = mapData[y - 1][x] == '#';
        }

        // Wybierz odpowiednią teksturę
        SDL_Texture *textureToUse =
            hasAbove ? plainBlockTexture : grassBlockTexture;

        blocks.emplace_back(x * tileSize, y * tileSize, renderer, textureToUse);
      } else if (mapData[y][x] == 'p') {
        playerMapX = x * tileSize;
        playerMapY = y * tileSize;
      } else if (mapData[y][x] == 'E') {
        exitExists = true;
        exitRect = {x * tileSize, y * tileSize, tileSize, tileSize};
      }
    }
  }

  // Ustaw wymiary świata gry na podstawie wczytanej mapy
  worldWidth = maxCol * tileSize;
  worldHeight = row * tileSize;

  // Zapewnij minimalny rozmiar świata
  worldWidth = std::max(worldWidth, 480);
  worldHeight = std::max(worldHeight, 272);
}

void Map::render(SDL_Renderer *renderer, const Camera *camera) {
  // Render blocks with camera offset
  for (auto &block : blocks) {
    // Let each block handle its own camera-aware rendering
    block.render(renderer, camera);
  }

  // Render exit with camera offset
  if (exitExists) {
    SDL_Rect exitScreenRect = exitRect;
    
    if (camera) {
      exitScreenRect.x -= static_cast<int>(camera->getOffsetX());
      exitScreenRect.y -= static_cast<int>(camera->getOffsetY());
    }

    if (exitTexture) {
      SDL_RenderCopy(renderer, exitTexture, NULL, &exitScreenRect);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderFillRect(renderer, &exitScreenRect);

      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawRect(renderer, &exitScreenRect);
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
