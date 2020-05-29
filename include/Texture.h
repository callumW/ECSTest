#ifndef TEXTURE_H
#define TEXTURE_H
#include <SDL2/SDL.h>

typedef struct texture_t {
    SDL_Texture* tex = nullptr;
    SDL_Rect src_rect = {};

    int width() const { return src_rect.w; }
    int height() const { return src_rect.h; }
} texture_t;
#endif
