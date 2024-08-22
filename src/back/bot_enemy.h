#ifndef BOT_ENEMY_H
#define BOT_ENEMY_H

#include "command.h"
#include "geometry.h"
#include "player.h"
#include "ship.h"
#include <math.h>
#include <stdlib.h>

#define AI_COOLDOWN 100 // ai turret cooldown



// Emulate a command creation for the bot ship.
// It is assumed that the bot has a left_pointing ship.
void bot_action(player_t *ai_player, int *cooldown_counter,
                player_t *human_player, shell_list *list);

double calculate_angle_to_player(ship *ai_ship, ship *human_ship);

// estimate travel distance for shell
double simulate_shell_flight(ship *ai_ship, double angle, int initial_speed);

#endif // BOT_ENEMY_H
