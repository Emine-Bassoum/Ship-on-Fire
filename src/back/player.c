#include "player.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

player_t *create_player(char *name, bool is_human) {

    player_t *res = malloc(sizeof(player_t)); // result
    assert(res != NULL);

    res->name = name;
    res->is_human = is_human;
    res->command.type = COMMAND_NONE;
    res->state = PLAYER_STATE_IDLE;

    return res;
}

void free_player(player_t *player) {
    free(player);
    return;
}

void execute_player_command(player_t *player, shell_list *list) {

    // command ordered by the player
    player_command_type command_type = player->command.type;

    switch (command_type) {

    case COMMAND_NONE:

        break;

    // change the chadburn setting
    case COMMAND_SPEED:
        switch (player->command.speed_param.action) {

        case INCREASE:
            // increase but cap setting to max value
            if (player->controlled_ship->chadburn_setting <
                CHADBURN_SPEED_FULL_AHEAD) {
                player->controlled_ship->chadburn_setting++;
            }
            break;

        case DECREASE:
            // decrease but cap setting to min value
            if (player->controlled_ship->chadburn_setting >
                CHADBURN_SPEED_FULL_REVERSE) {
                player->controlled_ship->chadburn_setting--;
            }
            break;

        default:
            fprintf(stderr,
                    "execute_player_command: wrong COMMAND_SPEED parameter "
                    "action (%d)\n",
                    player->command.speed_param.action);
            exit(EXIT_FAILURE);
        }
        break;

    case COMMAND_AIM:
        // this command should only be used when aiming
        assert(player->state == PLAYER_STATE_AIMING);
        // angle the cannon immediately to the wanted angle
        player->controlled_turret->cannon_angle =
            player->command.aim_param.target_angle;

        break;

    case COMMAND_SHOOT:
        // this command should only be used when aiming
        assert(player->state == PLAYER_STATE_AIMING);
        player->state = PLAYER_STATE_IDLE;
        shell *bullet = create_shell(player->controlled_turret);
        shell_list_add_element(list, bullet);

    case COMMAND_SELECT_PROJECTILE:

        break;

    case COMMAND_ENTER_AIM_MODE:;

        // number of the turret that will start aiming
        int new_turret_id = player->command.enter_aim_mode_param.turret_number;

        // assert that this turret exists on ship
        assert(0 <= new_turret_id);
        assert(new_turret_id < player->controlled_ship->turret_number);

        player->controlled_turret =
            player->controlled_ship->turrets[new_turret_id];
        player->state = PLAYER_STATE_AIMING; // start aiming

        break;

    case COMMAND_EXIT_AIM_MODE:
        // this command should only be used when aiming
        assert(player->state == PLAYER_STATE_AIMING);
        player->state = PLAYER_STATE_IDLE;

        break;
    }
}
