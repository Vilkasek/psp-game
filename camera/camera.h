#pragma once
#include <SDL2/SDL.h>

class Camera {
public:
  Camera(int screenWidth, int screenHeight, int worldWidth, int worldHeight);
  ~Camera();

  void update(float targetX, float targetY, float deltaTime);

  float getOffsetX() const;
  float getOffsetY() const;

  SDL_Rect worldToScreen(const SDL_Rect &worldRect) const;

  void setSmoothingFactor(float smoothing);

  void setBounds(int worldWidth, int worldHeight);

private:
  float x, y;
  int screenWidth, screenHeight;
  int worldWidth, worldHeight;
  float smoothingFactor;
};
