#include "block.h"

Block::Block(int x, int y, const char* filename, SDL_Window *window)
    : sprite(filename, window) {
    sprite.setPosition(x, y);
    rect = { x, y, sprite.getWidth(), sprite.getHeight() };
}

void Block::render(SDL_Renderer* renderer) {
    sprite.render(renderer);
}

SDL_Rect Block::getRect() const {
    return rect;
}

