#include "map.h"
#include <fstream>
#include <iostream>

Map::Map(SDL_Renderer *renderer, SDL_Texture *blockTexture)
    : renderer(renderer), texture(blockTexture) {}

void Map::loadFromFile(const char *filename) {
  blocks.clear();
  std::ifstream file(filename);

  if (!file.is_open()) {
    std::cerr << "Failed to open map file: " << filename << std::endl;
    return;
  }

  std::string line;
  int row = 0;

  while (std::getline(file, line)) {
    for (int col = 0; col < line.length(); ++col) {
      if (line[col] == '#') {
        blocks.emplace_back(col * 16, row * 16, renderer, texture);
        std::cout << "Added block at position: " << col * 16 << ", " << row * 16
                  << std::endl;
      }
    }
    row++;
  }

  std::cout << "Loaded " << blocks.size() << " blocks from map file"
            << std::endl;
}

void Map::render(SDL_Renderer *renderer) {
  for (auto &block : blocks) {
    block.render(renderer);
  }
}

const std::vector<Block> &Map::getBlocks() const { return blocks; }
