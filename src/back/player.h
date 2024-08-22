/* Data about the players defined here. */

#ifndef PLAYER_H
#define PLAYER_H

#include "command.h"
#include "ship.h"
#include "weapon.h"

enum player_state {
    PLAYER_STATE_IDLE, // player is not aiming
    PLAYER_STATE_AIMING,
};

// forward declarations, needed because of recursive structs
typedef struct turret turret;
typedef struct ship ship;
typedef struct shell_list shell_list;

typedef struct player_t {
    char *name; // string representing player name
    bool is_human;

    command_t command; // last command issued by the input / the AI

    // describes the state of the player, see back/player.h
    enum player_state state;

    /* Pointer to the turret that the player is controling.
     * Relevant only if player.state = PLAYER_STATE_AIMING */
    turret *controlled_turret;

    ship *controlled_ship; // pointer to ship
} player_t;

/* Create a new player object. */
player_t *create_player(char *name, bool is_human);

// free memory reserved for player
void free_player(player_t *player);

/* receive the commands or/and manage it
use a fonction initialising the bullets */
void execute_player_command(player_t *player, shell_list *list);

#endif /* PLAYER_H */
