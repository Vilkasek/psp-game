#include "map.h"
#include <fstream>
#include <iostream>

Map::Map(SDL_Renderer *renderer, SDL_Texture *blockTexture)
    : renderer(renderer), 
      texture(blockTexture), 
      exitTexture(nullptr),
      exitExists(false) {}

void Map::setExitTexture(SDL_Texture* texture) {
    exitTexture = texture;
}

// Zmodyfikuj metodę loadFromFile aby obsługiwała wyjścia ('E')
void Map::loadFromFile(const char *filename) {
  blocks.clear();
  exitExists = false; // Resetujemy stan wyjścia przy ładowaniu nowej mapy

  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Failed to open map file: " << filename << std::endl;
    return;
  }

  std::string line;
  int row = 0;

  while (std::getline(file, line)) {
    for (int col = 0; col < line.length(); ++col) {
      if (line[col] == '#') {
        blocks.emplace_back(col * 16, row * 16, renderer, texture);
        std::cout << "Added block at position: " << col * 16 << ", " << row * 16
                  << std::endl;
      } else if (line[col] == 'p') {
        playerMapX = col * 16;
        playerMapY = row * 16;
      } else if (line[col] == 'E') {
        // Dodaj wyjście
        exitExists = true;
        exitRect = {col * 16, row * 16, 16, 16};
        std::cout << "Added exit at position: " << col * 16 << ", " << row * 16
                  << std::endl;
      }
    }
    row++;
  }

  std::cout << "Loaded " << blocks.size() << " blocks from map file"
            << std::endl;
  if (exitExists) {
    std::cout << "Exit found at position: " << exitRect.x << ", " << exitRect.y
              << std::endl;
  }
}

// Dodaj renderowanie wyjścia
void Map::render(SDL_Renderer *renderer) {
  for (auto &block : blocks) {
    block.render(renderer);
  }
  
  // Renderuj wyjście
  if (exitExists) {
    if (exitTexture) {
      // Użyj tekstury wyjścia, jeśli jest dostępna
      SDL_RenderCopy(renderer, exitTexture, NULL, &exitRect);
    } else {
      // Fallback do prostokąta, jeśli tekstura nie jest dostępna
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Zielony kolor dla wyjścia
      SDL_RenderFillRect(renderer, &exitRect);
      
      // Dodaj obramowanie dla lepszej widoczności
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
