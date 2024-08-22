#include "input_handler.h"
#include "../back/geometry.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

void fetch_input(input_t *input) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            input->quit = 1;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
                input->up = 1;
                break;
            case SDLK_DOWN:
                input->down = 1;
                break;
            case SDLK_LEFT:
                input->left = 1;
                break;
            case SDLK_RIGHT:
                input->right = 1;
                break;
            case SDLK_q:
                input->q = 1;
                break;
            case SDLK_d:
                input->d = 1;
                break;
            case SDLK_f:
                input->f = 1;
                break;
            case SDLK_SPACE:
                input->space = 1;
                break;
            case SDLK_ESCAPE:
                input->escape = 1;
                break;
            }
            break;

        case SDL_KEYUP:
            switch (event.key.keysym.sym) {
            case SDLK_UP:
                input->up = 0;
                break;
            case SDLK_DOWN:
                input->down = 0;
                break;
            case SDLK_LEFT:
                input->left = 0;
                break;
            case SDLK_RIGHT:
                input->right = 0;
                break;
            case SDLK_q:
                input->q = 0;
                break;
            case SDLK_d:
                input->d = 0;
                break;
            case SDLK_f:
                input->f = 0;
                break;
            case SDLK_SPACE:
                input->space = 0;
                break;
            case SDLK_ESCAPE:
                input->escape = 0;
                break;
            }
            break;

        case SDL_MOUSEMOTION:
            input->mouse_x = event.motion.x;
            input->mouse_y = event.motion.y;
            break;

        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_LEFT) {
                input->mouse_left = 1;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                input->mouse_right = 1;
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_LEFT) {
                input->mouse_left = 0;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                input->mouse_right = 0;
            }
            break;
        }
    }
}

void handle_player_input(input_t input, input_t previous_input,
                         player_t *player, interface_button **all_buttons,
                         camera_t *cam, bool *game_running) {

    // default setting
    player->command.type = COMMAND_NONE;

    ////////////////////////// keyboard-related input //////////////////////////

    if (input.escape || input.quit) {
        printf("Exiting game.\n");
        *game_running = false;
        return;
    }
    if (input.up) {
        printf("Up key is pressed\n");
    }
    if (input.down) {
        printf("Down key is pressed\n");
    }
    if (input.space && !previous_input.space) {
        // switch camera target
        if (cam->ship_target == 1) {
            cam->ship_target = 2;
        } else if (cam->ship_target == 2) {
            cam->ship_target = 1;
        } else { // wrong camera target
            fprintf(stderr,
                    "\nError in handle_player_input: wrong camera target");
            exit(EXIT_FAILURE);
        }
    }
    if (input.right) {
        // printf("Right key is pressed\n");
        if (cam->mode == CAMERA_MODE_FREE) {
            cam->rect.x += 10;
        }
    }
    if (input.left) {
        // printf("Left key is pressed\n");
        if (cam->mode == CAMERA_MODE_FREE) {
            cam->rect.x -= 10;
        }
    }
    // alternative to button click for chadburn decrease
    if (input.q && all_buttons[0]->click_cooldown == 0) {
        // set "clicked" texture
        all_buttons[0]->texture_rect->x = all_buttons[0]->texture_rect->w;
        // reset cooldown
        all_buttons[0]->click_cooldown = BUTTON_DEFAULT_CLICK_COOLDOWN;
        player->command.type = COMMAND_SPEED;
        player->command.speed_param.action = DECREASE;
        return;
    }
    // alternative to button click for chadburn increase
    if (input.d && all_buttons[1]->click_cooldown == 0) {
        // set "clicked" texture
        all_buttons[1]->texture_rect->x = all_buttons[1]->texture_rect->w;
        // reset cooldown
        all_buttons[1]->click_cooldown = BUTTON_DEFAULT_CLICK_COOLDOWN;
        player->command.type = COMMAND_SPEED;
        player->command.speed_param.action = INCREASE;
        return;
    }
    // alternative to button click to enter aim mode
    if (input.f && all_buttons[2]->click_cooldown == 0) {
        // set "clicked" texture
        all_buttons[2]->texture_rect->x = all_buttons[2]->texture_rect->w;
        // reset cooldown
        all_buttons[2]->click_cooldown = BUTTON_DEFAULT_CLICK_COOLDOWN;
        player->command.type = COMMAND_ENTER_AIM_MODE;
        player->command.enter_aim_mode_param.turret_number = 0;
    }
    /////////////////////////// mouse-related input ///////////////////////////

    // on-screen mouse cursor
    SDL_Point cursor = {
        .x = input.mouse_x,
        .y = input.mouse_y,
    };

    // new left click somewhere
    if (input.mouse_left && !previous_input.mouse_left) {

        bool a_button_was_clicked = false;

        // buttons are tested for click
        for (int i = 0; i < BUTTON_NUMBER; i++) {
            interface_button *button = all_buttons[i];

            // if the button cooldown is over and the mouse is on the button
            if (button->click_cooldown == 0 &&
                SDL_PointInRect(&cursor, &(button->rect))) {

                a_button_was_clicked = true;

                // set "clicked" texture
                button->texture_rect->x = button->texture_rect->w;
                // reset cooldown
                button->click_cooldown = BUTTON_DEFAULT_CLICK_COOLDOWN;

                switch (button->type) {

                case BUTTON_TYPE_CHADBURN_DECREASE:
                    player->command.type = COMMAND_SPEED;
                    player->command.speed_param.action = DECREASE;
                    break;

                case BUTTON_TYPE_CHADBURN_INCREASE:
                    player->command.type = COMMAND_SPEED;
                    player->command.speed_param.action = INCREASE;
                    break;

                case BUTTON_TYPE_CANNON_1:
                    // printf("Cannon 1 button pressed!");
                    player->command.type = COMMAND_ENTER_AIM_MODE;
                    player->command.enter_aim_mode_param.turret_number = 0;
                    break;

                case BUTTON_TYPE_CANNON_2:
                    // printf("Cannon 2 button pressed!");
                    player->command.type = COMMAND_ENTER_AIM_MODE;
                    player->command.enter_aim_mode_param.turret_number = 1;
                    break;

                case BUTTON_TYPE_PAUSE_MENU:
                    printf("Pause button pressed!");
                    break;
                }
                // no need to test other buttons since they don't overlap
                return;
            }
        }

        // if player clicked while aiming and didn't click a button,
        // they may want to shoot
        if (player->state == PLAYER_STATE_AIMING && !a_button_was_clicked) {

            // center view of the level
            SDL_Rect main_view_rect = (SDL_Rect){
                .x = 0,
                .y = 0,
                .w = WINDOW_WIDTH,
                .h = MAIN_VIEW_HEIGHT,
            };

            // player will shoot
            if (SDL_PointInRect(&cursor, &main_view_rect)) {
                player->command.type = COMMAND_SHOOT;
                return;
            }
        }
    }
    // if player aiming and cursor moved
    if (player->state == PLAYER_STATE_AIMING &&
        (input.mouse_x != previous_input.mouse_x ||
         input.mouse_y != previous_input.mouse_y)) {

        // center view of the level
        SDL_Rect main_view_rect = (SDL_Rect){
            .x = 0, .y = 0, .w = WINDOW_WIDTH, .h = MAIN_VIEW_HEIGHT};

        // if cursor still in main view: change aim
        if (SDL_PointInRect(&cursor, &main_view_rect)) {

            player->command.type = COMMAND_AIM;

            // absolute (back) coordinates of the shell_starting_point of the
            // turret controlled by the player
            SDL_Point abs_turret = shift_point(
                player->controlled_turret->turret_pivot,
                shift_point(
                    rect_coords(player->controlled_turret->relative_position),
                    player->controlled_ship->coordinates));

            // on-screen coordinates of the shell_starting_point of the
            // turret controlled by the player
            SDL_Point screen_turret = ingame_coords_to_screen(abs_turret, *cam);

            // not a straight angle
            if (cursor.x - screen_turret.x != 0) {

                player->command.aim_param.target_angle = rad_to_deg(atan2(
                    cursor.y - screen_turret.y, cursor.x - screen_turret.x));

                // allow left-pointing ships to aim correctly (360 degrees
                // translating enable correct min/max angle comparisons)
                if (player->controlled_ship->point_left &&
                    player->command.aim_param.target_angle < 0) {
                    player->command.aim_param.target_angle += 360;
                }

            } else { // cursor.x - screen_turret.x == 0 : straight angle
                if (cursor.y >= screen_turret.y) { // cursor above turret
                    player->command.aim_param.target_angle = 45.0;
                } else { // cursor under turret
                    player->command.aim_param.target_angle = -45.0;
                }
            }
            // cap angle to maximum value
            float max_angle = player->controlled_turret->max_cannon_angle;
            if (player->command.aim_param.target_angle > max_angle) {
                player->command.aim_param.target_angle = max_angle;
            }
            // cap angle to minimum value
            float min_angle = player->controlled_turret->min_cannon_angle;
            if (player->command.aim_param.target_angle < min_angle) {
                player->command.aim_param.target_angle = min_angle;
            }
            return;
        }
    }
}
