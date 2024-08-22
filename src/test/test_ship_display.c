#include "../back/ship.h"
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

    SDL_Point spawn = {0, 0};
    SDL_Point spawn2 = {0, 200}; // en haut

    ship *boat = create_new_ship(NULL, SHIP_TYPE_CORVETTE, spawn, renderer, false);
    ship *boat2 = create_new_ship(NULL, SHIP_TYPE_CORVETTE, spawn2, renderer, true);

    bool running = true;

    while (running) {
        fetch_input(&input);
        (boat->turrets[0]->cannon_angle) += 1;
        (boat2->turrets[0]->cannon_angle) -= 1;

        draw_ship(boat, renderer, camera);
        draw_ship(boat2, renderer, camera);
        draw_ship_hitbox(boat, renderer, camera);
        draw_ship_hitbox(boat2, renderer, camera);
        SDL_RenderPresent(renderer);
        if (input.q) {
            running = false;
        }
    }

    free_ship(boat);
    free_ship(boat2);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
