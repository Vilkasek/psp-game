#include "camera.h"
#include <algorithm>

Camera::Camera(int screenWidth, int screenHeight, int worldWidth,
               int worldHeight)
    : x(0), y(0), screenWidth(screenWidth), screenHeight(screenHeight),
      worldWidth(worldWidth), worldHeight(worldHeight), smoothingFactor(0.1f) {}

Camera::~Camera() {}

void Camera::update(float targetX, float targetY, float deltaTime) {
  float targetCameraX = targetX - screenWidth / 2.0f;
  float targetCameraY = targetY - screenHeight / 2.0f;

  x += (targetCameraX - x) * smoothingFactor * (deltaTime * 10.0f);
  y += (targetCameraY - y) * smoothingFactor * (deltaTime * 10.0f);

  x = std::max(0.0f, std::min(x, static_cast<float>(worldWidth - screenWidth)));
  y = std::max(0.0f,
               std::min(y, static_cast<float>(worldHeight - screenHeight)));
}

float Camera::getOffsetX() const { return x; }

float Camera::getOffsetY() const { return y; }

SDL_Rect Camera::worldToScreen(const SDL_Rect &worldRect) const {
  SDL_Rect screenRect = worldRect;
  screenRect.x -= static_cast<int>(x);
  screenRect.y -= static_cast<int>(y);
  return screenRect;
}

void Camera::setSmoothingFactor(float smoothing) {
  smoothingFactor = std::max(0.01f, std::min(1.0f, smoothing));
}

void Camera::setBounds(int worldWidth, int worldHeight) {
  this->worldWidth = worldWidth;
  this->worldHeight = worldHeight;
}
