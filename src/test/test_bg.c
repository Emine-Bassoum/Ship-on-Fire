#include "../front/background.h"
#include "../front/input_handler.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {
    camera_t camera = {
        .rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
        .mode = CAMERA_MODE_FIXED,
        .ship_target = 1,
    };

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Window *window = SDL_CreateWindow(
        "Ship on FIRE!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (!window) {
        printf("Window error");
        exit(EXIT_FAILURE);
    }
    // rendering with Vsync on for Framerate to sync with screen frequency
    SDL_Renderer *renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_SOFTWARE);

    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n",
               SDL_GetError());
        exit(EXIT_FAILURE);
    }
    input_t input = {0};
    background_graphic bg_graphic;
    init_background(renderer, &bg_graphic);

    bool running = true;
    while (running) {
        fetch_input(&input);
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);
        update_bg_display_pos(&bg_graphic, camera);
        draw_background(renderer, &bg_graphic);
        SDL_RenderPresent(renderer);
        if (input.q) {
            running = false;
        }
    }
    free_background(&bg_graphic);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
