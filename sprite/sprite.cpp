#include "sprite.h"
#include <SDL2/SDL_render.h>

Sprite::Sprite(const char *filename, SDL_Window *window)
    : filename(filename), window(window), renderer(SDL_GetRenderer(window)) {
  loadTexture();
  SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
  rect = {rect.x, rect.y, getWidth(), getHeight()};
}
Sprite::Sprite(SDL_Texture *texture) {
  this->texture = texture;
  SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
  rect = {rect.x, rect.y, getWidth(), getHeight()};
}

Sprite::~Sprite() { unloadTexture(); }

void Sprite::setPosition(int x, int y) {
  rect.x = x;
  rect.y = y;
}

int Sprite::getWidth() const { return rect.w; }

int Sprite::getHeight() const { return rect.h; }

bool Sprite::loadTexture() {
  texture = loadTexture(filename, window);
  return texture != nullptr;
}

void Sprite::render(SDL_Renderer *renderer) const {
  SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Sprite::unloadTexture() {
  if (texture != nullptr) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
}

SDL_Texture *Sprite::loadTexture(const char *filename, SDL_Window *window) {
  SDL_Surface *pixels = IMG_Load(filename);
  if (pixels == nullptr) {
    return nullptr;
  }

  SDL_Texture *sprite =
      SDL_CreateTextureFromSurface(SDL_GetRenderer(window), pixels);
  SDL_FreeSurface(pixels);

  return sprite;
}

SDL_Rect Sprite::getRect() { return rect; }
