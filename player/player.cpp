#include "player.h"
#include <SDL2/SDL_rect.h>

Player::Player(const char *filename, SDL_Window *window)
    : sprite(filename, window) {}

Player::~Player() {}

void Player::setPosition(int x, int y) {
  this->x = static_cast<float>(x);
  this->y = static_cast<float>(y);
  sprite.setPosition(x, y);
}

void Player::render(SDL_Renderer *renderer) const { sprite.render(renderer); }

int Player::getX() const { return static_cast<int>(x); }

int Player::getY() const { return static_cast<int>(y); }

float Player::getVelocityX() const { return velocityX; }

float Player::getVelocityY() const { return velocityY; }

bool Player::getIsOnGround() const { return isOnGround; }

int Player::getWidth() const { return sprite.getWidth(); }

int Player::getHeight() const { return sprite.getHeight(); }

void Player::move(SDL_GameController *controller, float deltaTime,
                  const Block &block) {
  float prevVelocityX = velocityX;
  float prevVelocityY = velocityY;

  bool left =
      SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
  bool right =
      SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
  bool jump = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);

  if (left && !right) {
    velocityX -= acceleration * deltaTime;
    if (velocityX < -speed)
      velocityX = -speed;
  } else if (right && !left) {
    velocityX += acceleration * deltaTime;
    if (velocityX > speed)
      velocityX = speed;
  } else {
    if (velocityX > 0) {
      velocityX -= deceleration * deltaTime;
      if (velocityX < 0)
        velocityX = 0;
    } else if (velocityX < 0) {
      velocityX += deceleration * deltaTime;
      if (velocityX > 0)
        velocityX = 0;
    }
  }

  velocityY += gravity * deltaTime;

  if (jump && isOnGround) {
    velocityY = jumpForce;
    isOnGround = false;
  }

  float maxFallSpeed = 500.0f;
  if (velocityY > maxFallSpeed) {
    velocityY = maxFallSpeed;
  }

  x += velocityX * deltaTime;
  y += velocityY * deltaTime;

  const int screenWidth = 480;
  const int screenHeight = 272;

  if (x < 0) {
    x = 0;
    velocityX = 0;
  } else if (x > screenWidth - getWidth()) {
    x = screenWidth - getWidth();
    velocityX = 0;
  }

  if (y > screenHeight - getHeight()) {
    y = screenHeight - getHeight();
    velocityY = 0;
    isOnGround = true;
  } else {
    isOnGround = false;
  }

  SDL_Rect playerRect = getRect();
  SDL_Rect blockRect = block.getRect();

  if (SDL_HasIntersection(&playerRect, &blockRect)) {
    if (velocityY > 0 &&
        playerRect.y + playerRect.h <= blockRect.y + velocityY) {
      y = blockRect.y - playerRect.h;
      velocityY = 0;
      isOnGround = true;
    } else if (velocityY < 0 &&
               playerRect.y >= blockRect.y + blockRect.h + velocityY) {
      y = blockRect.y + blockRect.h;
      velocityY = 0;
    } else {
      velocityX = 0;
    }
  }

  sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
}

SDL_Rect Player::getRect() { return sprite.getRect(); }
