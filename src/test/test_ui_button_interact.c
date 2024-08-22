#include "../front/input_handler.h"
#include "../front/user_interface.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <stdio.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {

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
    input_t previous_input;

    chadburn_graphic mychadburn_graphic;
    init_chadburn_graphic(&mychadburn_graphic, renderer);

    interface_button **all_buttons = create_all_buttons(renderer);

    bool running = true;
    while (running) {
        previous_input = input;
        fetch_input(&input);

        // code from input_handler

        SDL_Point cursor = {
            .x = input.mouse_x,
            .y = input.mouse_y,
        };

        // new left click somewhere
        if (input.mouse_left && !previous_input.mouse_left) {

            // bool a_button_was_clicked = false;

            for (int i = 0; i < BUTTON_NUMBER; i++) {
                interface_button *button = all_buttons[i];

                // if the button cooldown is over and the mouse is on the button
                if (button->click_cooldown == 0 &&
                    SDL_PointInRect(&cursor, &(button->rect))) {

                    // a_button_was_clicked = true;

                    // set "clicked" texture
                    button->texture_rect->x = button->texture_rect->w;
                    // reset cooldown
                    button->click_cooldown = BUTTON_DEFAULT_CLICK_COOLDOWN;
                }
            }
        }

        button_cooldown(all_buttons);

        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        draw_chadburn_graphic(renderer, &mychadburn_graphic);
        for (int i = 0; i < BUTTON_NUMBER; i++) {
            interface_button *button = all_buttons[i];

            // render button
            SDL_RenderCopy(renderer, button->texture, button->texture_rect,
                           &(button->rect));

            // render box
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            SDL_RenderDrawRect(renderer, &button->rect);
        }
        SDL_RenderPresent(renderer);
        if (input.q) {
            running = false;
        }
    }
    free_all_buttons(all_buttons);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    return 0;
}
