#pragma once
#include "../camera/camera.h"
#include "../sprite/sprite.h"
#include <SDL2/SDL.h>
#include <memory>

class Block {
public:
  Block(int x, int y, const char *filename, SDL_Window *window);

  Block(int x, int y, SDL_Renderer *renderer, SDL_Texture *texture);

  // Modified to accept camera for proper rendering
  void render(SDL_Renderer *renderer, const Camera *camera = nullptr);
  SDL_Rect getRect() const;

private:
  int blockSize = 32;
  std::unique_ptr<Sprite> sprite;
  SDL_Rect rect;
  SDL_Texture *texture = nullptr;
  SDL_Renderer *renderer = nullptr;
  bool useSprite = true;
};
