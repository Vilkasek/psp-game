#include "block.h"
#include <iostream>

Block::Block(int x, int y, const char* filename, SDL_Window *window)
    : sprite(new Sprite(filename, window)), useSprite(true) {
    sprite->setPosition(x, y);
    rect = { x, y, sprite->getWidth(), sprite->getHeight() };
}

Block::Block(int x, int y, SDL_Renderer *renderer, SDL_Texture *texture)
    : sprite(nullptr), renderer(renderer), texture(texture), useSprite(false) {
    // Determine texture size
    int w, h;
    if (SDL_QueryTexture(texture, NULL, NULL, &w, &h) != 0) {
        std::cerr << "Failed to query texture: " << SDL_GetError() << std::endl;
        w = 16;
        h = 16;
    }
    
    // Use a standard block size of 16x16 pixels
    rect = { x, y, 16, 16 };
}

void Block::render(SDL_Renderer* renderer) {
    if (useSprite && sprite) {
        sprite->render(renderer);
    } else {
        // Render using the texture directly
        if (texture != nullptr) {
            SDL_RenderCopy(renderer, texture, NULL, &rect);
        } else {
            // Fallback rendering if texture is null
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

SDL_Rect Block::getRect() const {
    return rect;
}
