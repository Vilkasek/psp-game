#pragma once
#include "../sprite/sprite.h"
#include <SDL2/SDL.h>

class Block {
public:
  Block(int x, int y, const char* filename, SDL_Window *window);
  void render(SDL_Renderer *renderer);
  SDL_Rect getRect() const;

private:
  Sprite sprite;
  SDL_Rect rect;
};
