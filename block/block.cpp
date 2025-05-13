#include "block.h"

Block::Block(int x, int y, const char *filename, SDL_Window *window)
    : sprite(new Sprite(filename, window)), useSprite(true) {
  sprite->setPosition(x, y);
  rect = {x, y, sprite->getWidth(), sprite->getHeight()};
}

Block::Block(int x, int y, SDL_Renderer *renderer, SDL_Texture *texture)
    : sprite(nullptr), renderer(renderer), texture(texture), useSprite(false) {
  int w, h;
  if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) != 0) {
    w = blockSize;
    h = blockSize;
  }

  rect = {x, y, blockSize, blockSize};
}

void Block::render(SDL_Renderer *renderer) {
  if (useSprite && sprite) {
    sprite->render(renderer);
  } else {
    if (texture != nullptr) {
      SDL_RenderCopy(renderer, texture, NULL, &rect);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderFillRect(renderer, &rect);
    }
  }
}

SDL_Rect Block::getRect() const { return rect; }
