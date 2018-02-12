#include <SDL2/SDL.h>
#include <math.h>
#include "window.h"

#define APP_NAME "chip8emu"
#define SCREEN_WIDTH  64
#define SCREEN_HEIGHT 32

static SDL_Window   *window;
static SDL_Renderer *renderer;

void window_init(bool fullscreen) {
    uint8_t screen_flags = fullscreen
        ? SDL_WINDOW_OPENGL|SDL_WINDOW_FULLSCREEN
        : SDL_WINDOW_OPENGL;

    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_ShowCursor(SDL_FALSE);

    window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, screen_flags);

    renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, 0);
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void window_draw(uint8_t *pixels) {
    uint16_t i;
    uint8_t x;
    uint8_t y;

    if (!pixels)
        goto PRESENT;

    SDL_RenderClear(renderer);

    for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        x = i % SCREEN_WIDTH;
        y = (uint8_t)(i / SCREEN_WIDTH);

        if (pixels[i]) {
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

PRESENT:
    SDL_RenderPresent(renderer);
}

void window_free(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
}
