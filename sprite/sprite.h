#ifndef SPRITE_H
#define SPRITE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>

class Sprite {
public:
  Sprite(const char *filename, SDL_Window *window);
  Sprite(SDL_Texture *texture);
  ~Sprite();

  void setPosition(int x, int y);
  void render(SDL_Renderer *renderer) const;

  int getWidth() const;
  int getHeight() const;

  SDL_Rect getRect();

  SDL_Texture *getTexture() const { return texture; }

  // Dodane funkcje do obsługi flipowania
  void setFlip(SDL_RendererFlip flip);
  SDL_RendererFlip getFlip() const;

private:
  SDL_Texture *texture = nullptr;
  SDL_Rect rect = {};
  // Dodane pole do przechowywania informacji o flipowaniu
  SDL_RendererFlip flip = SDL_FLIP_NONE;

  bool loadTexture();
  SDL_Texture *loadTexture(const char *filename, SDL_Window *window);
  void unloadTexture();

  const char *filename;
  SDL_Window *window = nullptr;
  SDL_Renderer *renderer = nullptr;
};

#endif // SPRITE_H
