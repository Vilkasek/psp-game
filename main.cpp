#include "block/block.h"
#include "player/player.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *gameController = nullptr;

bool initSDL();
SDL_Window *createWindow(const char *title, int width, int height);
bool gameLoop(Player &player, Block &block);
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

  Player player("grass.png", window);
  Block block(10, 200, "grass.png", window);
  player.setPosition(480 / 2 - player.getWidth() / 2,
                     272 / 2 - player.getHeight() / 2);

  if (!gameLoop(player, block)) {
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

bool gameLoop(Player &player, Block &block) {
  bool running = true;
  SDL_Event event;

  Uint32 lastTime = SDL_GetTicks();
  Uint32 currentTime;

  float deltaTime;

  while (running) {
    currentTime = SDL_GetTicks();

    deltaTime = (currentTime - lastTime) / 1000.f;

    lastTime = currentTime;

    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      case SDL_CONTROLLERDEVICEADDED:
        SDL_GameControllerOpen(event.cdevice.which);
        break;
      case SDL_CONTROLLERBUTTONDOWN:
        if (event.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
          running = false;
        }
        break;
      }
    }

    if (gameController) {
      player.move(gameController, deltaTime, block);
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    block.render(renderer);
    player.render(renderer);
    SDL_RenderPresent(renderer);
  }

  return true;
}

void cleanSDL() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}
