#ifndef PLAYER_H
#define PLAYER_H

#include "../block/block.h"
#include "../sprite/sprite.h"
#include <SDL2/SDL.h>

class Player {
public:
  Player(const char *filename, SDL_Window *window);
  ~Player();

  void setPosition(int x, int y);
  void render(SDL_Renderer *renderer) const;

  int getX() const;
  int getY() const;
  int getWidth() const;
  int getHeight() const;

  float getVelocityX() const;
  float getVelocityY() const;
  bool getIsOnGround() const;

  SDL_Rect getRect();

  void checkCollision(const Block &block);
  void move(SDL_GameController *controller, float deltaTime);

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
};

#endif // PLAYER_H
