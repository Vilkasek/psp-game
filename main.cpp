#include "block/block.h"
#include "level_manager/level_manager.h"
#include "map/map.h"
#include "player/player.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
SDL_GameController *gameController = nullptr;

int currentLevel = 1;
const int MAX_LEVELS = 2;

bool initSDL();
SDL_Window *createWindow(const char *title, int width, int height);
bool gameLoop(Player &player, Map &map, LevelManager &levelManager);
void cleanSDL();

int main(int argc, char *argv[]) {
  if (!initSDL()) {
    return -1;
  }

  window = createWindow("Platform Game", 480, 272);
  if (window == nullptr) {
    return -1;
  }

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    return -1;
  }

  SDL_RenderSetVSync(renderer, 1);

  Player player("player.png", window);

  SDL_Texture *blockTexture = IMG_LoadTexture(renderer, "wall.png");
  SDL_Texture *exitTexture = IMG_LoadTexture(renderer, "grass.png");

  if (!blockTexture) {
    return -1;
  }

  if (!exitTexture) {
    return -1;
  }

  Map map(renderer, blockTexture);
  map.setExitTexture(exitTexture);

  LevelManager levelManager(renderer, blockTexture);
  levelManager.setExitTexture(exitTexture);

  if (!levelManager.initialize("maps/")) {
    return -1;
  }

  if (!levelManager.loadLevel(1, player, map)) {
    return -1;
  }

  if (!gameLoop(player, map, levelManager)) {
    return -1;
  }

  SDL_DestroyTexture(blockTexture);
  if (exitTexture) {
    SDL_DestroyTexture(exitTexture);
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

bool gameLoop(Player &player, Map &map, LevelManager &levelManager) {
  bool running = true;
  SDL_Event event;

  Uint32 lastTime = SDL_GetTicks();
  Uint32 currentTime;

  float deltaTime;

  const float MAX_DELTA_TIME = 0.05f;

  while (running) {
    currentTime = SDL_GetTicks();

    deltaTime = (currentTime - lastTime) / 1000.f;

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
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        } else if (event.key.keysym.sym == SDLK_n) {
          if (!levelManager.goToNextLevel(player, map)) {
            if (levelManager.isGameCompleted()) {
              running = false;
            }
          }
        }
        break;
      }
    }

    player.move(gameController, deltaTime);

    player.resetGroundState();

    for (const Block &block : map.getBlocks()) {
      player.checkCollision(block);
    }

    if (map.hasExit() && map.checkExitCollision(player.getRect())) {
      if (!levelManager.goToNextLevel(player, map)) {
        if (levelManager.isGameCompleted()) {
          running = false;
        }
      }
    }

    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderClear(renderer);

    map.render(renderer);

    player.render(renderer);

    static int lastRenderedLevel = 0;
    if (lastRenderedLevel != levelManager.getCurrentLevel()) {
      lastRenderedLevel = levelManager.getCurrentLevel();
    }

    SDL_RenderPresent(renderer);

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
