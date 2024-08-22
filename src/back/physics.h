/* Functions related to the physical game engine are declared here.*/

#ifndef PHYSICS_H
#define PHYSICS_H

#include "ship.h"

/**
 * Update the position, speed and acceleration of a ship.
 * The chadburn setting is used to update the desired speed.
 */
void update_ship_kinematics(ship *ship);

/* Update the speed of a shell. */
void update_shell_kinematics(shell *shell);

/* Set target_speed according to the current position of the chadburn. */
void set_ship_target_speed(ship *ship);

/* Update kinematics and health of ships colliding.
 * Use elastic collisions preserving total momentum. */
void apply_ship_on_ship_collisions(ship *ship_1, ship *ship_2);

#endif
