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

  Player player("grass.png", window);

  SDL_Texture *map_texture = IMG_LoadTexture(renderer, "grass.png");

  Map map(renderer, map_texture);
  map.loadFromFile("maps/level1.txt");

  player.setPosition(480 / 2 - player.getWidth() / 2,
                     272 / 2 - player.getHeight() / 2);

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
      case SDL_KEYDOWN:
        // Handle keyboard input for testing
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          running = false;
        }
        break;
      }
    }

    // Handle keyboard movement for testing if no controller available
    const Uint8 *keyState = SDL_GetKeyboardState(NULL);
    if (!gameController && keyState) {
      // Simulate controller input with keyboard
      if (keyState[SDL_SCANCODE_LEFT]) {
        player.setPosition(player.getX() - 2, player.getY());
      }
      if (keyState[SDL_SCANCODE_RIGHT]) {
        player.setPosition(player.getX() + 2, player.getY());
      }
      if (keyState[SDL_SCANCODE_UP]) {
        player.setPosition(player.getX(), player.getY() - 2);
      }
      if (keyState[SDL_SCANCODE_DOWN]) {
        player.setPosition(player.getX(), player.getY() + 2);
      }
    }

    if (gameController) {
      player.move(gameController, deltaTime);
    }

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
  }

  return true;
}
void cleanSDL() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
}
