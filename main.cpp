#include "block/block.h"
#include "map/map.h"
#include "player/player.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <iostream>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *gameController = nullptr;

bool initSDL();
SDL_Window *createWindow(const char *title, int width, int height);
bool gameLoop(Player &player, Map &map);
void cleanSDL();

int main(int argc, char *argv[]) {
  if (!initSDL()) {
    return -1;
  }

  window = createWindow("window", 480, 272);
  if (window == nullptr) {
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    return -1;
  }

  SDL_RenderSetVSync(renderer, 1);

  Player player("player.png", window);

  SDL_Texture *map_texture = IMG_LoadTexture(renderer, "grass.png");

  Map map(renderer, map_texture);
  map.loadFromFile("maps/level1.txt");

  player.setPosition(static_cast<int>(map.getPlayerMapX()),
                     static_cast<int>(map.getPlayerMapY()));

  if (!gameLoop(player, map)) {
    return -1;
  }

  cleanSDL();
  return 0;
}

bool initSDL() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
    return false;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    return false;
  }

  for (int i = 0; i < SDL_NumJoysticks(); i++) {
    if (SDL_IsGameController(i)) {
      gameController = SDL_GameControllerOpen(i);
      if (gameController) {
        break;
      }
    }
  }

  return true;
}

SDL_Window *createWindow(const char *title, int width, int height) {
  SDL_Window *window =
      SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       width, height, 0);
  return window;
}

bool gameLoop(Player &player, Map &map) {
  bool running = true;
  SDL_Event event;

  Uint32 lastTime = SDL_GetTicks();
  Uint32 currentTime;

  float deltaTime;

  // Display number of blocks loaded
  std::cout << "Game loop started with " << map.getBlocks().size()
            << " blocks loaded" << std::endl;

  // Ogranicz maksymalny deltaTime aby uniknąć problemów przy dużym lagu
  const float MAX_DELTA_TIME = 0.05f; // maksymalnie 50ms

  while (running) {
    currentTime = SDL_GetTicks();

    deltaTime = (currentTime - lastTime) / 1000.f;

    // Ogranicz maksymalny deltaTime
    if (deltaTime > MAX_DELTA_TIME) {
      deltaTime = MAX_DELTA_TIME;
    }

    lastTime = currentTime;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_CONTROLLERDEVICEADDED:
        gameController = SDL_GameControllerOpen(event.cdevice.which);
        break;
      case SDL_CONTROLLERDEVICEREMOVED:
        if (gameController &&
            SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(
                gameController)) == event.cdevice.which) {
          SDL_GameControllerClose(gameController);
          gameController = nullptr;
        }
        break;
      case SDL_CONTROLLERBUTTONDOWN:
        if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
          running = false;
        }
        break;
      case SDL_KEYDOWN:
        // Handle keyboard input for testing
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
        break;
      }
    }

    // Aktualizacja pozycji gracza
    player.move(gameController, deltaTime);

    // Resetujemy flagę isOnGround przed sprawdzeniem kolizji
    player.resetGroundState();

    // Sprawdź kolizje po ruchu gracza
    for (const Block &block : map.getBlocks()) {
      player.checkCollision(block);
    }

    // Set background color to make blocks more visible
    SDL_SetRenderDrawColor(renderer, 100, 100, 200,
                           255); // Light blue background
    SDL_RenderClear(renderer);

    // Render the map first (so blocks appear behind the player)
    map.render(renderer);

    // Then render the player on top
    player.render(renderer);

    SDL_RenderPresent(renderer);

    // Dodaj małe opóźnienie aby oszczędzić CPU
    SDL_Delay(1);
  }

  return true;
}

void cleanSDL() {
  if (gameController) {
    SDL_GameControllerClose(gameController);
    gameController = nullptr;
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}
