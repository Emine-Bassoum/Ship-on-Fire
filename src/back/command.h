/* Data and functions related to commands are defined here. */

#ifndef COMMAND_H
#define COMMAND_H

/* The commands are defined here :

    the commands have a type (COMMAND_SPEED, COMMAND_ATTACK...)
    and may have parameters that depend on the type. The parameters should
    and may have parameters that depend on the type. The parameters should
    be accessed after testing the type.

    List of the parameters:

    COMMAND_NONE: no parameters
    (do nothing)

    COMMAND_SPEED, command_speed_parameters:
        int chadburn_setting:
            speed set among CHADBURN_FULL_REVERSE,
            CHADBURN_HALF_REVERSE, CHADBURN_NEUTRAL, CHADBURN_THIRD_AHEAD,
            CHADBURN_TWO_THIRDS_AHEAD, CHADBURN_FULL_AHEAD (see back/ship.h).
    (the player sets a speed on the chadburn)

    COMMAND_AIM, command_aim_parameters:
        float target_angle:
            angle in degrees that player.currently_controling_cannon is supposed
            to aim towards

    COMMAND_SHOOT: no parameters
    (player.currently_controling_cannon will spawn a projectile)

    COMMAND_SELECT_PROJECTILE:
        // TODO

    COMMAND_ENTER_AIM_MODE, command_enter_aim_mode_parameters:
        int turret_number:
            number of the turret that is supposed to be aiming

    COMMAND_EXIT_AIM_MODE: no parameters

 */

// types of commands a player can issue
typedef enum {
    COMMAND_NONE,  // do nothing
    COMMAND_SPEED, // set the chadburn speed
    COMMAND_AIM,   // change cannon angle
    COMMAND_SHOOT,
    COMMAND_SELECT_PROJECTILE,
    COMMAND_ENTER_AIM_MODE,
    COMMAND_EXIT_AIM_MODE
} player_command_type;

typedef enum {
    INCREASE,
    DECREASE,
} chadburn_action;

typedef struct {
    chadburn_action action;
} command_speed_parameters;

typedef struct {
    int target_angle; // in degrees
} command_aim_parameters;

typedef struct {
    int turret_number; // number of the turret that must be used
} command_enter_aim_mode_parameters;

/* Representation of a player command.*/
typedef struct {
    player_command_type type;
    union {
        command_speed_parameters speed_param;
        command_aim_parameters aim_param;
        command_enter_aim_mode_parameters enter_aim_mode_param;
    };
} command_t;

#endif
