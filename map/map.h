#pragma once
#include "../block/block.h"
#include <vector>

class Map {
public:
  Map(SDL_Renderer *renderer, SDL_Texture *blockTexture);

  void loadFromFile(const char *filename);
  void render(SDL_Renderer *renderer);

  float getPlayerMapX();
  float getPlayerMapY();
  
  const std::vector<Block> &getBlocks() const;
  
  // Metody do obsługi wyjść/przejść
  bool hasExit() const;
  SDL_Rect getExitRect() const;
  void clearExit();
  bool checkExitCollision(SDL_Rect playerRect) const;
  
  // Dodaj metodę do ustawienia tekstury wyjścia
  void setExitTexture(SDL_Texture* texture);

private:
  std::vector<Block> blocks;
  SDL_Texture *texture;     // Tekstura dla bloków
  SDL_Texture *exitTexture; // Tekstura dla wyjścia
  SDL_Renderer *renderer;

  float playerMapX;
  float playerMapY;
  
  // Struktura dla wyjścia/przejścia
  bool exitExists;
  SDL_Rect exitRect;
};
