/* Ship-related structures are defined here. */

#ifndef SHIP_H
#define SHIP_H
#include "../front/camera.h"
#include "player.h"
#include "weapon.h"
#include <SDL2/SDL_rect.h>

// forward declarations
typedef struct turret turret;    // needed because of recursive structs
typedef struct shell shell;      // needed because of recursive file inclusions
typedef int turret_type;         // needed because of recursive file inclusions
typedef int on_ship_orientation; // because of recursive file inclusions
typedef struct player_t player_t;


// scale between texture size and real ship size
#define SHIP_SCALE 2

/* Authorised chadburn settings*/
typedef enum {
    CHADBURN_SPEED_FULL_REVERSE,
    CHADBURN_SPEED_HALF_REVERSE,
    CHADBURN_SPEED_NEUTRAL,
    CHADBURN_SPEED_THIRD_AHEAD,
    CHADBURN_SPEED_TWO_THIRDS_AHEAD,
    CHADBURN_SPEED_FULL_AHEAD,
} chadburn_speed;

// types of ship
typedef enum {
    SHIP_TYPE_CORVETTE,
    SHIP_TYPE_1, // placeholder type
    SHIP_TYPE_2, // placeholder type
    SHIP_TYPE_3, // placeholder type
    SHIP_TYPE_4, // placeholder type
} ship_type;

// Default ship information for a particular ship type
// (see default_ship_data)
typedef struct {

    // health
    int max_hull_health;
    int max_crew_health;
    int armor;

    // kinematic constants
    int max_speed;     // maximum forwards ship speed
    int max_rev_speed; // maximum backwards ship speed
    int max_acc;       // max acceleration of a ship
    int weight;        // Weight of a ship

    SDL_Rect hitbox; // hull hitbox: relative to top left of ship

    // graphical data
    int width;                  // ship texture width
    int height;                 // ship texture height
    SDL_Rect hull_texture_rect; // hull texture position in texture sheet

    // turret data
    int turret_number;         // number of turrets equipped on the ship
    SDL_Point *turret_coord;   // array; relative to top left of ship
    turret_type *turrets_type; // array
    on_ship_orientation *turret_orientation; // array

} ship_type_data;

// Holds all the data regarding the ship.
typedef struct ship {

    ///////////////////////////// static data /////////////////////////////////

    player_t *owner_player;

    // health
    int max_hull_health; // initial and maximal health value of the hull
    int max_crew_health; // initial and maximal health value of the crew

    // kinematic constants
    int max_speed;     // maximum forwards ship speed
    int max_rev_speed; // maximum backwards ship speed
    int max_acc;       // max acceleration of a ship
    int weight;        // Weight of a ship

    // hull hitbox: relative to real coordinates
    SDL_Rect hitbox;

    // graphical data
    int width;                  // ship texture width
    int height;                 // ship texture height
    SDL_Texture *hull_texture;  // pointer to hull texture sheet
    SDL_Rect hull_texture_rect; // hull texture position in texture sheet
    bool point_left;            // true if the ship is oriented to the left

    ////////////////////// (static & dynamic) turret data //////////////////////

    int turret_number; // number of turrets present on the ship
    // pointer to (shared) turret texture sheet, can be NULL if no turrets exist
    SDL_Texture *turret_texture;
    turret **turrets; // array of pointers to turrets

    /////////////////////////////// dynamic data ///////////////////////////////

    // kinematic data
    SDL_Point coordinates; // in-game coordinates
    SDL_Point speed;       // in-game speed
    SDL_Point acc;         // ship acceleration

    chadburn_speed chadburn_setting; // used to compute the ship's target speed
    int target_speed;                // speed targetted by the chadburn
    float accumulated_speed_change;  // used to accumulate speed changes
    // health data
    int hull_health;  // current health value of the hull
    int crew_health;  // current health value of the crew
    int time_on_fire; // TODO : documentation
    int armor;        // TODO : documentation

    // misc data
    int state; // TODO : documentation
} ship;

/* Return all data needed to initialize a ship with a given type. */
ship_type_data default_ship_data(ship_type type);

/* Free a ship_type_data from memory. */
void free_ship_type_data(ship_type_data data);

/** Return a pointer to a newly created and initialized ship.
 * @param coordinates Spawning position
 * @param point_left True if the ship should be facing the left side
 */
ship *create_new_ship(player_t *owner, ship_type s_type, SDL_Point coordinates,
                      SDL_Renderer *renderer, bool point_left);

/* Free heap memory allocated for the ship structure, including the turrets, but
 * not the owner_player. */
void free_ship(ship *target);

/**
 * Draw a ship on a rendering target.
 * @param ship_to_draw Pointer to the ship structure
 * @param renderer SDL renderer to draw the ship_graphic
 * @param cam
 */
void draw_ship(ship *ship_to_draw, SDL_Renderer *renderer, camera_t cam);

// draw the hitbox of a ship
void draw_ship_hitbox(ship *s, SDL_Renderer *renderer, camera_t camera);

/**
 * Inflict damage to a ship that is hit by a shell. */
void inflict_shell_dmg(ship *target, shell *incoming);

#endif /* SHIP_H */
