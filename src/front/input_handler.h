#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include "../back/command.h"
#include "../back/player.h"
#include "camera.h"
#include "user_interface.h"
#include <SDL2/SDL.h>

// representation of a computer input.
typedef struct {
    int up;
    int down;
    int left;
    int right;
    int quit; // Alt-F4 or other system-dependant equivalent
    int q;
    int d;
    int f;
    int space;
    int escape;
    int mouse_x;
    int mouse_y;
    int mouse_left;
    int mouse_right;
} input_t;

// Update the input object with the physical input
void fetch_input(input_t *input);

/**
 * Convert a human input into a command, set the player's command field, and
 * update non-gameplay settings, such as camera, pause menu, game exit...
 * See also execute_player_command() in player.h.
 * @param input Current input
 * @param previous_input Input that was fetched last frame
 * @param player Pointer to player that is modified
 * @param all_buttons buttons used in interface
 * @param cam Pointer to camera
 * @param game_running Boolean that can be set to false to stop the game
 */
void handle_player_input(input_t input, input_t previous_input,
                         player_t *player, interface_button **all_buttons,
                         camera_t *cam, bool *game_running);

#endif // INPUT_HANDLER_H