#include "player.h"
#include <SDL2/SDL_rect.h>
#include <iostream>

Player::Player(const char *filename, SDL_Window *window)
    : sprite(filename, window) {}

Player::~Player() {}

void Player::setPosition(int x, int y) {
  this->x = static_cast<float>(x);
  this->y = static_cast<float>(y);
  sprite.setPosition(x, y);
}

void Player::render(SDL_Renderer *renderer, const Camera *camera) const {
  SDL_Rect playerRect = {static_cast<int>(x), static_cast<int>(y),
                         sprite.getWidth(), sprite.getHeight()};
  SDL_Rect screenRect;

  if (camera) {
    screenRect.x = playerRect.x - static_cast<int>(camera->getOffsetX());
    screenRect.y = playerRect.y - static_cast<int>(camera->getOffsetY());
    screenRect.w = playerRect.w;
    screenRect.h = playerRect.h;
  } else {
    screenRect = playerRect;
  }

  if (sprite.getTexture()) {
    // Ustawienie odpowiedniego flipowania w zależności od kierunku
    SDL_RendererFlip flip = facingRight ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
    SDL_RenderCopyEx(renderer, sprite.getTexture(), NULL, &screenRect, 0.0,
                     NULL, flip);
  } else {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &screenRect);
  }
}

int Player::getX() const { return static_cast<int>(x); }

int Player::getY() const { return static_cast<int>(y); }

float Player::getVelocityX() const { return velocityX; }

float Player::getVelocityY() const { return velocityY; }

bool Player::getIsOnGround() const { return isOnGround; }

int Player::getWidth() const { return sprite.getWidth(); }

int Player::getHeight() const { return sprite.getHeight(); }

void Player::updateDirection() {
  // Aktualizacja kierunku gracza na podstawie prędkości poziomej
  if (velocityX > 0) {
    facingRight = true;
  } else if (velocityX < 0) {
    facingRight = false;
  }
  // Jeśli prędkość jest 0, zachowujemy obecny kierunek
}

void Player::checkCollision(const Block &block) {
  SDL_Rect playerRect = getRect();
  SDL_Rect blockRect = block.getRect();

  if (SDL_HasIntersection(&playerRect, &blockRect)) {
    int penetrationLeft = (blockRect.x + blockRect.w) - playerRect.x;
    int penetrationRight = (playerRect.x + playerRect.w) - blockRect.x;
    int penetrationTop = (blockRect.y + blockRect.h) - playerRect.y;
    int penetrationBottom = (playerRect.y + playerRect.h) - blockRect.y;

    if (penetrationTop <= penetrationBottom &&
        penetrationTop <= penetrationLeft &&
        penetrationTop <= penetrationRight && velocityY < 0) {
      y = blockRect.y + blockRect.h;
      velocityY = 0;
    } else if (penetrationBottom <= penetrationTop &&
               penetrationBottom <= penetrationLeft &&
               penetrationBottom <= penetrationRight && velocityY > 0) {
      y = blockRect.y - playerRect.h;
      velocityY = 0;
      isOnGround = true;
    } else if (penetrationLeft <= penetrationRight &&
               penetrationLeft <= penetrationTop &&
               penetrationLeft <= penetrationBottom && velocityX < 0) {
      x = blockRect.x + blockRect.w;
      velocityX = 0;
    } else if (penetrationRight <= penetrationLeft &&
               penetrationRight <= penetrationTop &&
               penetrationRight <= penetrationBottom && velocityX > 0) {
      x = blockRect.x - playerRect.w;
      velocityX = 0;
    } else {
      if (playerRect.y + playerRect.h / 2 <= blockRect.y + blockRect.h / 2) {
        y = blockRect.y - playerRect.h;
        velocityY = 0;
        isOnGround = true;
      }
    }

    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
  }
}

void Player::updateTimers(float deltaTime) {
  if (isOnGround) {
    wasOnGroundLastFrame = true;
    currentCoyoteTime = coyoteTime;
  } else if (wasOnGroundLastFrame) {
    wasOnGroundLastFrame = false;
  } else {
    if (currentCoyoteTime > 0) {
      currentCoyoteTime -= deltaTime;
    }
  }

  if (jumpBuffered) {
    if (currentJumpBufferTime > 0) {
      currentJumpBufferTime -= deltaTime;
      if (currentJumpBufferTime <= 0) {
        jumpBuffered = false;
      }
    }
  }
}

void Player::move(SDL_GameController *controller, float deltaTime,
                  int worldWidth, int worldHeight) {
  updateTimers(deltaTime);

  bool left = false;
  bool right = false;
  bool jump = false;

  if (controller) {
    left = SDL_GameControllerGetButton(controller,
                                       SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    right = SDL_GameControllerGetButton(controller,
                                        SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    jump = SDL_GameControllerGetButton(controller, SDL_CONTROLLER_BUTTON_A);
  }

  // Sprawdź klawiaturę
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  if (keyboardState[SDL_SCANCODE_LEFT])
    left = true;
  if (keyboardState[SDL_SCANCODE_RIGHT])
    right = true;
  if (keyboardState[SDL_SCANCODE_SPACE] || keyboardState[SDL_SCANCODE_UP])
    jump = true;

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

  if (jump) {
    if (isOnGround || currentCoyoteTime > 0) {
      velocityY = jumpForce;
      isOnGround = false;
      currentCoyoteTime = 0;
      jumpBuffered = false;
      std::cout << "Jump executed! Ground: " << (isOnGround ? "true" : "false")
                << ", Coyote time: " << currentCoyoteTime << std::endl;
    } else {
      jumpBuffered = true;
      currentJumpBufferTime = jumpBufferTime;
      std::cout << "Jump buffered!" << std::endl;
    }
  } else if (jumpBuffered && (isOnGround || currentCoyoteTime > 0)) {
    velocityY = jumpForce;
    isOnGround = false;
    currentCoyoteTime = 0;
    jumpBuffered = false;
    std::cout << "Buffered jump executed!" << std::endl;
  }

  float maxFallSpeed = 500.0f;
  if (velocityY > maxFallSpeed) {
    velocityY = maxFallSpeed;
  }

  x += velocityX * deltaTime;
  y += velocityY * deltaTime;

  // Sprawdzaj kolizje z granicami świata zamiast stałych wymiarów ekranu
  if (x < 0) {
    x = 0;
    velocityX = 0;
  } else if (x > worldWidth - getWidth()) {
    x = worldWidth - getWidth();
    velocityX = 0;
  }

  if (y > worldHeight - getHeight()) {
    y = worldHeight - getHeight();
    velocityY = 0;
    isOnGround = true;
  }

  // Aktualizacja kierunku gracza
  updateDirection();

  sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
}

SDL_Rect Player::getRect() const {
  return {static_cast<int>(x), static_cast<int>(y), sprite.getWidth(),
          sprite.getHeight()};
}
