#pragma once
#include "../block/block.h"
#include <vector>

class Map {
public:
  Map(SDL_Renderer *renderer, SDL_Texture *blockTexture);
  void loadFromFile(const char *filename);
  void render(SDL_Renderer *renderer);
  const std::vector<Block> &getBlocks() const;

private:
  std::vector<Block> blocks;
  SDL_Texture *texture;
  SDL_Renderer *renderer;
};
