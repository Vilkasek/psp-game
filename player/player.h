#ifndef PLAYER_H
#define PLAYER_H

#include "../block/block.h"
#include "../camera/camera.h"
#include "../sprite/sprite.h"
#include <SDL2/SDL.h>

class Player {
public:
  Player(const char *filename, SDL_Window *window);
  ~Player();

  void setPosition(int x, int y);
  void render(SDL_Renderer *renderer, const Camera *camera = nullptr) const;

  int getX() const;
  int getY() const;
  int getWidth() const;
  int getHeight() const;

  float getVelocityX() const;
  float getVelocityY() const;
  bool getIsOnGround() const;

  void resetGroundState() { isOnGround = false; }

  SDL_Rect getRect() const;

  void checkCollision(const Block &block);

  void move(SDL_GameController *controller, float deltaTime, int worldWidth,
            int worldHeight);

  void updateTimers(float deltaTime);

private:
  Sprite sprite;
  float x = 0.0f;
  float y = 0.0f;
  float velocityX = 0.0f;
  float velocityY = 0.0f;
  float speed = 150.0f;
  float acceleration = 800.0f;
  float deceleration = 600.0f;
  float gravity = 980.0f;
  float jumpForce = -400.0f;
  bool isOnGround = false;

  float coyoteTime = 0.15f;
  float currentCoyoteTime = 0.0f;
  bool wasOnGroundLastFrame = false;

  float jumpBufferTime = 0.1f;
  float currentJumpBufferTime = 0.0f;
  bool jumpBuffered = false;

  bool pointInRect(float px, float py, const SDL_Rect &rect) {
    return (px >= rect.x && px <= rect.x + rect.w && py >= rect.y &&
            py <= rect.y + rect.h);
  }
};

#endif // PLAYER_H
