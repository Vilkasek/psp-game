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

void Player::render(SDL_Renderer *renderer) const { sprite.render(renderer); }

int Player::getX() const { return static_cast<int>(x); }

int Player::getY() const { return static_cast<int>(y); }

float Player::getVelocityX() const { return velocityX; }

float Player::getVelocityY() const { return velocityY; }

bool Player::getIsOnGround() const { return isOnGround; }

int Player::getWidth() const { return sprite.getWidth(); }

int Player::getHeight() const { return sprite.getHeight(); }

void Player::checkCollision(const Block &block) {
  SDL_Rect playerRect = getRect();
  SDL_Rect blockRect = block.getRect();

  if (SDL_HasIntersection(&playerRect, &blockRect)) {
    // Określamy kierunek kolizji na podstawie głębokości penetracji
    // w każdym z czterech kierunków
    int penetrationLeft = (blockRect.x + blockRect.w) - playerRect.x;
    int penetrationRight = (playerRect.x + playerRect.w) - blockRect.x;
    int penetrationTop = (blockRect.y + blockRect.h) - playerRect.y;
    int penetrationBottom = (playerRect.y + playerRect.h) - blockRect.y;

    // Znajdź najmniejszą penetrację, aby określić kierunek kolizji
    if (penetrationTop <= penetrationBottom &&
        penetrationTop <= penetrationLeft &&
        penetrationTop <= penetrationRight && velocityY < 0) {
      // Kolizja od góry (gracz uderza w spód bloku)
      y = blockRect.y + blockRect.h;
      velocityY = 0;
    } else if (penetrationBottom <= penetrationTop &&
               penetrationBottom <= penetrationLeft &&
               penetrationBottom <= penetrationRight && velocityY > 0) {
      // Kolizja od dołu (gracz ląduje na bloku)
      y = blockRect.y - playerRect.h;
      velocityY = 0;
      isOnGround = true;
    } else if (penetrationLeft <= penetrationRight &&
               penetrationLeft <= penetrationTop &&
               penetrationLeft <= penetrationBottom && velocityX < 0) {
      // Kolizja od lewej (gracz uderza w prawy bok bloku)
      x = blockRect.x + blockRect.w;
      velocityX = 0;
    } else if (penetrationRight <= penetrationLeft &&
               penetrationRight <= penetrationTop &&
               penetrationRight <= penetrationBottom && velocityX > 0) {
      // Kolizja od prawej (gracz uderza w lewy bok bloku)
      x = blockRect.x - playerRect.w;
      velocityX = 0;
    } else {
      // Jeśli prędkość jest bardzo mała i nie możemy określić kierunku
      // Ustaw gracza na górze bloku jako najbezpieczniejszą opcję
      if (playerRect.y + playerRect.h / 2 <= blockRect.y + blockRect.h / 2) {
        y = blockRect.y - playerRect.h;
        velocityY = 0;
        isOnGround = true;
      }
    }

    // Zaktualizuj pozycję sprite'a po zmianie pozycji
    sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
  }
}

// Nowa metoda do aktualizacji timerów
void Player::updateTimers(float deltaTime) {
  // Aktualizacja Coyote Time
  if (isOnGround) {
    wasOnGroundLastFrame = true;
    currentCoyoteTime =
        coyoteTime; // Resetuj czas coyote, gdy gracz jest na ziemi
  } else if (wasOnGroundLastFrame) {
    // Gracz właśnie opuścił ziemię, rozpocznij odliczanie coyote time
    wasOnGroundLastFrame = false;
    // currentCoyoteTime został już ustawiony przy kontakcie z ziemią
  } else {
    // Gracz jest w powietrzu i już był w powietrzu w poprzedniej klatce
    if (currentCoyoteTime > 0) {
      currentCoyoteTime -= deltaTime;
    }
  }

  // Aktualizacja Jump Buffer
  if (jumpBuffered) {
    if (currentJumpBufferTime > 0) {
      currentJumpBufferTime -= deltaTime;
      if (currentJumpBufferTime <= 0) {
        jumpBuffered = false; // Wygaśnięcie bufora skoku
      }
    }
  }
}

void Player::move(SDL_GameController *controller, float deltaTime) {
  // Aktualizuj timery
  updateTimers(deltaTime);

  // Odczytaj wejście od kontrolera (lub klawiatury)
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

  // Dodaj obsługę klawiatury niezależnie czy jest kontroler czy nie
  const Uint8 *keyState = SDL_GetKeyboardState(NULL);
  if (keyState) {
    left |= keyState[SDL_SCANCODE_LEFT];
    right |= keyState[SDL_SCANCODE_RIGHT];
    jump |= keyState[SDL_SCANCODE_SPACE] || keyState[SDL_SCANCODE_UP];
  }

  // Przetwarzaj ruch w poziomie
  if (left && !right) {
    velocityX -= acceleration * deltaTime;
    if (velocityX < -speed)
      velocityX = -speed;
  } else if (right && !left) {
    velocityX += acceleration * deltaTime;
    if (velocityX > speed)
      velocityX = speed;
  } else {
    // Spowolnienie przy braku ruchu
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

  // Aplikuj grawitację
  velocityY += gravity * deltaTime;

  // Obsługa Jump Buffera - jeśli gracz nacisnął przycisk skoku
  if (jump) {
    if (isOnGround || currentCoyoteTime > 0) {
      // Jeśli gracz jest na ziemi lub w okresie coyote time, wykonaj skok od
      // razu
      velocityY = jumpForce;
      isOnGround = false;
      currentCoyoteTime = 0; // Reset coyote time po skoku
      jumpBuffered = false;  // Reset jump buffer po wykonaniu skoku
      std::cout << "Jump executed! Ground: " << (isOnGround ? "true" : "false")
                << ", Coyote time: " << currentCoyoteTime << std::endl;
    } else {
      // Jeśli gracz nie jest na ziemi, buforuj żądanie skoku
      jumpBuffered = true;
      currentJumpBufferTime = jumpBufferTime;
      std::cout << "Jump buffered!" << std::endl;
    }
  }
  // Sprawdź, czy buforowany skok może być wykonany
  else if (jumpBuffered && (isOnGround || currentCoyoteTime > 0)) {
    velocityY = jumpForce;
    isOnGround = false;
    currentCoyoteTime = 0;
    jumpBuffered = false;
    std::cout << "Buffered jump executed!" << std::endl;
  }

  // Ogranicz maksymalną prędkość spadania
  float maxFallSpeed = 500.0f;
  if (velocityY > maxFallSpeed) {
    velocityY = maxFallSpeed;
  }

  // Przesuwamy gracza na podstawie aktualnej prędkości
  x += velocityX * deltaTime;
  y += velocityY * deltaTime;

  // Sprawdź granice ekranu
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
  }

  // Zaktualizuj pozycję sprite'a
  sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
}

SDL_Rect Player::getRect() { // Deklaracja pełnych wymiarów getRect z użyciem
                             // aktualnej pozycji x,y
  sprite.setPosition(static_cast<int>(x), static_cast<int>(y));
  return {static_cast<int>(x), static_cast<int>(y), sprite.getWidth(),
          sprite.getHeight()};
}
