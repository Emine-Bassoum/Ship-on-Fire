#include "back/bot_enemy.h"
#include "back/physics.h"
#include "back/player.h"
#include "back/ship.h"
#include "back/weapon.h"
#include "front/background.h"
#include "front/camera.h"
#include "front/input_handler.h"
#include "front/user_interface.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

int main(/*int argc, char *argv[]*/) {

    //////////////////////////// initialization ///////////////////////////////

    fprintf(stderr, "Initializing... ");

    // window & renderer

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_Window *window =
        SDL_CreateWindow("Ship on FIRE!", SDL_WINDOWPOS_CENTERED, 0,
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

    fprintf(stderr, "windows ok - ");

    // graphics

    // Initialize the camera
    camera_t camera = {
        .rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT},
        .mode = CAMERA_MODE_FIXED,
        .ship_target = 1, // view on player ship
    };

    // Initialize the background
    background_graphic bg_graphic;
    init_background(renderer, &bg_graphic);

    // Initialize the UI
    ui_bg_graphic ui_bg_graphic;
    init_ui_bg_graphic(&ui_bg_graphic, renderer);
    chadburn_graphic mychadburn_graphic;
    init_chadburn_graphic(&mychadburn_graphic, renderer);

    // buttons

    interface_button **all_buttons = create_all_buttons(renderer);

    fprintf(stderr, "graphics ok - ");

    // input

    input_t input = {0};
    input_t previous_input; // used to check if a value has changed

    // fps counter

    /*     FPSCounter fpsCounter;
        FPSCounter_Init(&fpsCounter);
    */

    //////// gameplay elements

    // players

    player_t *player1 = create_player("Human player", true);
    player_t *player2 = create_player("Bot", false);

    int ai_cooldown_counter = 0; // ai turret cooldown

    // ships

    SDL_Point spawn_1 = {0, 0};
    SDL_Point spawn_2 = {1000, 0};
    ship *ship_1, *ship_2;
    ship_1 =
        create_new_ship(player1, SHIP_TYPE_CORVETTE, spawn_1, renderer, false);
    ship_2 =
        create_new_ship(player2, SHIP_TYPE_CORVETTE, spawn_2, renderer, true);

    player1->controlled_ship = ship_1;
    player2->controlled_ship = ship_2;

    fprintf(stderr, "ships ok - ");

    // list of the shells that have to interact with the game
    shell_list *active_shells = shell_list_create(10);

    fprintf(stderr, "done!\n");

    // initialization done

    //////////////////////////////// main loop ////////////////////////////////

    bool game_running = true;
    while (game_running) {

        ///////////////////////////// FPS Counting /////////////////////////////

        /*
        FPSCounter_Tick(&fpsCounter);
        float fps = FPSCounter_GetFPS(&fpsCounter);
        printf("FPS: %.2f\n", fps);
        */

        ////////////////////// button cooldown handling ////////////////////////

        button_cooldown(all_buttons);

        //////////////////////////// input handling ////////////////////////////

        previous_input = input; // save previous input
        fetch_input(&input);

        // update human player with input data
        handle_player_input(input, previous_input, player1, all_buttons,
                            &camera, &game_running);

        /////////////////////////////// gameplay ///////////////////////////////

        execute_player_command(player1, active_shells);

        // emulate human input
        bot_action(player2, &ai_cooldown_counter, player1, active_shells);

        /////////////////////////////// physics ///////////////////////////////

        // ships

        // kinematics update
        update_ship_kinematics(ship_1);
        update_ship_kinematics(ship_2);

        // collisions
        apply_ship_on_ship_collisions(ship_1, ship_2);

        // shells

        // kinematics update
        shell_list_map(active_shells, update_shell_kinematics);

        // destroy shells that are in the water
        shell_list_remove_elements(active_shells, is_shell_in_water);

        // test collisions
        apply_shell_collisions(active_shells, ship_1);
        apply_shell_collisions(active_shells, ship_2);

        ///////////////////////////// game ending /////////////////////////////

        if (ship_1->crew_health <= 0 || ship_1->hull_health <= 0) {
            printf("Oh no! You lost... try again soon!\n");
            game_running = false;
        }
        if (ship_2->crew_health <= 0 || ship_2->hull_health <= 0) {
            printf("Congratulations! You won! Thanks for playing!\n");
            game_running = false;
        }

        //////////////////////////////// Camera ////////////////////////////////

        // update background graphic position
        update_bg_display_pos(&bg_graphic, camera);

        // if the camera is fixed on a ship
        if (camera.mode == CAMERA_MODE_FIXED) {

            // select the ship that is
            ship *ship_target;
            if (camera.ship_target == 1) {
                ship_target = ship_1;
            } else if (camera.ship_target == 2) {
                ship_target = ship_2;
            }

            SDL_Rect cam_target = {
                ship_target->coordinates.x,
                ship_target->coordinates.y,
                ship_target->width,
                ship_target->height,
            };
            move_camera_towards_object(&cam_target, &camera);
        }

        //////////////////////////// SDL Rendering ////////////////////////////

        // clear the renderer
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        draw_background(renderer, &bg_graphic);

        draw_ship(ship_1, renderer, camera);
        // draw_ship_hitbox(ship_1, renderer, camera); // debug: show the hitbox

        draw_ship(ship_2, renderer, camera);
        // draw_ship_hitbox(ship_2, renderer, camera);

        draw_shell_list(active_shells, renderer, camera);

        // UI rendering

        // UI bottom bar background
        draw_ui_bg(renderer, &ui_bg_graphic);

        // chadburn
        set_speed_chadburn_graphic(&mychadburn_graphic,
                                   ship_1->chadburn_setting);
        draw_chadburn_graphic(renderer, &mychadburn_graphic);

        // buttons
        for (int i = 0; i < BUTTON_NUMBER; i++) {
            interface_button *button = all_buttons[i];

            // render button
            SDL_RenderCopy(renderer, button->texture, button->texture_rect,
                           &(button->rect));

            // SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0x00, 0xFF);
            // SDL_RenderFillRect(renderer, &button_rect);
        }

        SDL_RenderPresent(renderer);

    } /* while (game_running) */

    //////////////////////////// freeing of data ///////////////////////////////

    free_background(&bg_graphic);

    free_all_buttons(all_buttons);
    free_ui_bg_graphic(ui_bg_graphic);
    free_chadburn_graphic(mychadburn_graphic);

    shell_list_remove_elements(active_shells, true_func);
    shell_list_destroy(active_shells);

    free_player(player1);
    free_player(player2);

    free_ship(ship_1);
    free_ship(ship_2);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // removing SDL_Quit() fixes some SDL-related memory leaks on some
    // architectures

    // SDL_Quit();

    return 0;
}
