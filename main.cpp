#include "block/block.h"
#include "level_manager/level_manager.h"
#include "map/map.h"
#include "player/player.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <iostream>

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

  SDL_Texture *blockTexture = IMG_LoadTexture(renderer, "grass.png");
  SDL_Texture *exitTexture =
      IMG_LoadTexture(renderer, "grass.png"); // Załaduj teksturę wyjścia

  if (!blockTexture) {
    std::cerr << "Failed to load block texture: " << SDL_GetError()
              << std::endl;
    return -1;
  }

  // Jeśli tekstura wyjścia nie istnieje, użyjemy domyślnego zielonego
  // prostokąta
  if (!exitTexture) {
    std::cerr << "Failed to load exit texture, using default: "
              << SDL_GetError() << std::endl;
  }

  Map map(renderer, blockTexture);
  map.setExitTexture(exitTexture);

  LevelManager levelManager(renderer, blockTexture);
  levelManager.setExitTexture(exitTexture);

  if (!levelManager.initialize("maps/")) {
    std::cerr << "Failed to initialize level manager" << std::endl;
    return -1;
  }

  if (!levelManager.loadLevel(1, player, map)) {
    std::cerr << "Failed to load initial level" << std::endl;
    return -1;
  }

  if (!gameLoop(player, map, levelManager)) {
    return -1;
  }

  // Zwolnij tekstury przed wyjściem
  SDL_DestroyTexture(blockTexture);
  if (exitTexture) {
    SDL_DestroyTexture(exitTexture);
  }

  cleanSDL();
  return 0;
}

bool loadLevel(Player &player, Map &map, int levelNum) {
  char levelFile[100];
  sprintf(levelFile, "maps/level%d.txt", levelNum);

  // Sprawdź, czy plik istnieje
  FILE *file = fopen(levelFile, "r");
  if (!file) {
    std::cerr << "Level file does not exist: " << levelFile << std::endl;
    return false;
  }
  fclose(file);

  map.loadFromFile(levelFile);
  player.setPosition(static_cast<int>(map.getPlayerMapX()),
                     static_cast<int>(map.getPlayerMapY()));

  std::cout << "Loaded level " << levelNum << std::endl;
  return true;
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

  // Display number of blocks loaded
  std::cout << "Game loop started with " << map.getBlocks().size()
            << " blocks loaded on level " << levelManager.getCurrentLevel()
            << std::endl;

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
        // Dodaj możliwość przejścia do następnego poziomu za pomocą klawisza N
        // (do testów)
        else if (event.key.keysym.sym == SDLK_n) {
          if (!levelManager.goToNextLevel(player, map)) {
            if (levelManager.isGameCompleted()) {
              std::cout << "You've completed all levels! Game over."
                        << std::endl;
              running = false;
            }
          }
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

    // Sprawdź kolizję z wyjściem
    if (map.hasExit() && map.checkExitCollision(player.getRect())) {
      // Przejście do następnego poziomu
      if (!levelManager.goToNextLevel(player, map)) {
        if (levelManager.isGameCompleted()) {
          std::cout << "Congratulations! You completed all levels!"
                    << std::endl;
          running = false; // Koniec gry
        }
      }
    }

    // Set background color
    SDL_SetRenderDrawColor(renderer, 100, 100, 200,
                           255); // Light blue background
    SDL_RenderClear(renderer);

    // Render the map first (so blocks appear behind the player)
    map.render(renderer);

    // Then render the player on top
    player.render(renderer);

    // Wyświetl informację o aktualnym poziomie (w konsoli, ale można dodać
    // renderowanie tekstu)
    static int lastRenderedLevel = 0;
    if (lastRenderedLevel != levelManager.getCurrentLevel()) {
      lastRenderedLevel = levelManager.getCurrentLevel();
      std::cout << "Current level: " << lastRenderedLevel << std::endl;
    }

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
