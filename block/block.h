#pragma once
#include "../sprite/sprite.h"
#include <SDL2/SDL.h>
#include <memory>

class Block {
public:
  // Original constructor
  Block(int x, int y, const char *filename, SDL_Window *window);

  // New constructor for renderer and texture
  Block(int x, int y, SDL_Renderer *renderer, SDL_Texture *texture);

  void render(SDL_Renderer *renderer);
  SDL_Rect getRect() const;

private:
  std::unique_ptr<Sprite>
      sprite; // Change to pointer so we can initialize it conditionally
  SDL_Rect rect;
  SDL_Texture *texture = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool useSprite = true;
};
