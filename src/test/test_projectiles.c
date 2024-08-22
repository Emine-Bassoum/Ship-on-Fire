#include "../back/weapon.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <assert.h>

void test_create_shell(SDL_Renderer *renderer) {
    SDL_Point point = {0, 0};
    ship *boat = create_new_ship(NULL, SHIP_TYPE_CORVETTE, point, renderer, false);
    shell *new_shell = create_shell(boat->turrets[0]);
    // Check if new_shell is not NULL
    assert(new_shell != NULL);
    printf("Test 1 : SUCCESS ! \n");
    // Add more assertions to check the initial values of shell structure
    // members
    shell_destroy(new_shell); // Free the allocated memory for new_shell
    free_ship(boat);
}

// Test case for shell_list_create function
void test_shell_list_create() {
    shell_list *new_shell_list = shell_list_create(5);
    // Check if new_shell_list is not NULL
    assert(new_shell_list != NULL);
    // Check if the length of the list is correct
    assert(shell_list_length(new_shell_list) == 0);
    // Check if the maximum capacity of the list is correct
    assert(new_shell_list->max_capacity == 5);
    // Free the allocated memory for the shell list
    shell_list_destroy(new_shell_list);
    printf("Test 2 : SUCCESS ! \n");
}

// test adding function
void test_shell_list_add_element(SDL_Renderer *renderer) {
    SDL_Point point = {0, 0};
    ship *boat = create_new_ship(NULL, SHIP_TYPE_CORVETTE, point, renderer, false);
    shell_list *new_shell_list = shell_list_create(2);
    shell *new_shell0 = create_shell(boat->turrets[0]);
    shell *new_shell1 = create_shell(boat->turrets[0]);
    shell *new_shell2 = create_shell(boat->turrets[0]);
    shell *new_shell3 = create_shell(boat->turrets[0]);
    shell_list_add_element(new_shell_list, new_shell0);
    assert(shell_list_length(new_shell_list) == 1);
    shell_list_add_element(new_shell_list, new_shell1);
    assert(shell_list_length(new_shell_list) == 2);
    shell_list_add_element(new_shell_list, new_shell2);
    assert(shell_list_length(new_shell_list) == 3);
    shell_list_add_element(new_shell_list, new_shell3);
    assert(shell_list_length(new_shell_list) == 4);

    // Free the allocated memory
    free_ship(boat);
    shell_destroy(new_shell0);
    shell_destroy(new_shell1);
    shell_destroy(new_shell2);
    shell_destroy(new_shell3);

    shell_list_destroy(new_shell_list);

    printf("Test 3 : SUCCESS !\n");
}

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main() {

    // Run the tests

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

    test_create_shell(renderer);
    test_shell_list_create();
    test_shell_list_add_element(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    return 0;
}
