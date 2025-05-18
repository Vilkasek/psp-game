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

void Block::render(SDL_Renderer *renderer, const Camera *camera) {
  SDL_Rect renderRect = rect;

  // Apply camera offset if camera is provided
  if (camera) {
    renderRect.x -= static_cast<int>(camera->getOffsetX());
    renderRect.y -= static_cast<int>(camera->getOffsetY());
  }

  if (useSprite && sprite) {
    // Set sprite position adjusted for camera
    int originalX = sprite->getRect().x;
    int originalY = sprite->getRect().y;

    if (camera) {
      sprite->setPosition(originalX - static_cast<int>(camera->getOffsetX()),
                          originalY - static_cast<int>(camera->getOffsetY()));
    }

    sprite->render(renderer);

    // Restore original position
    sprite->setPosition(originalX, originalY);
  } else {
    if (texture != nullptr) {
      SDL_RenderCopy(renderer, texture, NULL, &renderRect);
    } else {
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderFillRect(renderer, &renderRect);
    }
  }
}

SDL_Rect Block::getRect() const { return rect; }
