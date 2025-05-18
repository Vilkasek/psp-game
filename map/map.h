#pragma once
#include "../block/block.h"
#include "../camera/camera.h"
#include <vector>

class Map {
public:
  // Zmodyfikowany konstruktor przyjmujący dwie tekstury
  Map(SDL_Renderer *renderer, SDL_Texture *grassBlockTexture,
      SDL_Texture *plainBlockTexture);

  void loadFromFile(const char *filename);
  void render(SDL_Renderer *renderer, const Camera *camera = nullptr);

  float getPlayerMapX();
  float getPlayerMapY();

  const std::vector<Block> &getBlocks() const;

  bool hasExit() const;
  SDL_Rect getExitRect() const;
  void clearExit();
  bool checkExitCollision(SDL_Rect playerRect) const;

  void setExitTexture(SDL_Texture *texture);

  // Dodane funkcje do obsługi wymiarów mapy
  int getWorldWidth() const;
  int getWorldHeight() const;

private:
  std::vector<Block> blocks;
  SDL_Texture *grassBlockTexture; // Tekstura z trawą
  SDL_Texture *plainBlockTexture; // Tekstura bez trawy
  SDL_Texture *exitTexture;
  SDL_Renderer *renderer;

  float playerMapX;
  float playerMapY;

  int tileSize = 32;

  bool exitExists;
  SDL_Rect exitRect;

  // Wymiary świata gry
  int worldWidth = 0;
  int worldHeight = 0;

  // Funkcja sprawdzająca, czy nad blokiem znajduje się inny blok
  bool hasBlockAbove(int x, int y) const;
};
