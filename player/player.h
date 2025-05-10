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

  // Resetuje stan bycia na ziemi na początku każdej klatki
  void resetGroundState() { isOnGround = false; }

  SDL_Rect getRect();

  // Sprawdza kolizję z blokiem i odpowiednio reaguje
  void checkCollision(const Block &block);

  // Obsługuje ruch gracza (zarówno z kontrolera jak i klawiatury)
  void move(SDL_GameController *controller, float deltaTime);

  // Aktualizuje timery dla coyote time i jump buffer
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

  // Coyote Time - czas po opuszczeniu platformy, gdy nadal można skoczyć
  float coyoteTime = 0.15f;       // Maksymalny czas coyote w sekundach
  float currentCoyoteTime = 0.0f; // Aktualny czas coyote
  bool wasOnGroundLastFrame =
      false; // Czy gracz był na ziemi w poprzedniej klatce

  // Jump Buffer - zapamiętuje żądanie skoku tuż przed wylądowaniem
  float jumpBufferTime = 0.1f; // Maksymalny czas bufora skoku w sekundach
  float currentJumpBufferTime = 0.0f; // Aktualny czas bufora skoku
  bool jumpBuffered = false;          // Czy żądanie skoku zostało zbuforowane

  // Metoda pomocnicza do sprawdzania czy punkt jest wewnątrz prostokąta
  bool pointInRect(float px, float py, const SDL_Rect &rect) {
    return (px >= rect.x && px <= rect.x + rect.w && py >= rect.y &&
            py <= rect.y + rect.h);
  }
};

#endif // PLAYER_H
