#include <SDL2/SDL.h>
#include "controls.h"

bool keyboard_event(SDL_Event *e, uint8_t *keys) {
    if (e->type == SDL_QUIT) {
        return false;
    }

    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_1: keys[0x1] = true; break;
            case SDLK_2: keys[0x2] = true; break;
            case SDLK_3: keys[0x3] = true; break;
            case SDLK_4: keys[0xC] = true; break;
            case SDLK_q: keys[0x4] = true; break;
            case SDLK_w: keys[0x5] = true; break;
            case SDLK_e: keys[0x6] = true; break;
            case SDLK_r: keys[0xD] = true; break;
            case SDLK_a: keys[0x7] = true; break;
            case SDLK_s: keys[0x8] = true; break;
            case SDLK_d: keys[0x9] = true; break;
            case SDLK_f: keys[0xE] = true; break;
            case SDLK_z: keys[0xA] = true; break;
            case SDLK_x: keys[0x0] = true; break;
            case SDLK_v: keys[0xB] = true; break;
            case SDLK_c: keys[0xF] = true; break;
        }
        return true;
    }

    if (e->type == SDL_KEYUP) {
        switch (e->key.keysym.sym) {
            case SDLK_1: keys[0x1] = false; break;
            case SDLK_2: keys[0x2] = false; break;
            case SDLK_3: keys[0x3] = false; break;
            case SDLK_4: keys[0xC] = false; break;
            case SDLK_q: keys[0x4] = false; break;
            case SDLK_w: keys[0x5] = false; break;
            case SDLK_e: keys[0x6] = false; break;
            case SDLK_r: keys[0xD] = false; break;
            case SDLK_a: keys[0x7] = false; break;
            case SDLK_s: keys[0x8] = false; break;
            case SDLK_d: keys[0x9] = false; break;
            case SDLK_f: keys[0xE] = false; break;
            case SDLK_z: keys[0xA] = false; break;
            case SDLK_x: keys[0x0] = false; break;
            case SDLK_v: keys[0xB] = false; break;
            case SDLK_c: keys[0xF] = false; break;
        }
        return true;
    }
    return true;
}

bool controls_poll_event(uint8_t *keys) {
    bool quit = false;
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        quit = !keyboard_event(&event, keys);
    }

    return quit;
}
