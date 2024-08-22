/* Data regarding the weapons and projectiles equipped on the ships defined
 * here. */

#ifndef WEAPON_H
#define WEAPON_H

#include "../front/camera.h"
#include "ship.h"
#include <SDL2/SDL_rect.h>
#include <stdbool.h>

// forward declarations
typedef struct ship ship; // needed because of recursive structs
typedef struct turret turret;

//////////////////////////////////// shells ////////////////////////////////////

/* Representation of a shell. */
typedef struct shell {

    ////////////// static data //////////////

    // pointer to the ship that fired the projectile
    ship *owner;

    int caliber;
    int power; // impacts initial size
    float accumulated_speed_change;

    // special attributes
    bool explosive;
    bool flare;
    bool smoke;
    bool piercing;
    bool incendiary;

    ////////// dynamic data ///////////

    // kinematic data
    SDL_Point coordinates; // absolute coordinates
    SDL_Point speed;

} shell;

// Create a new shell after it is shot.
shell *create_shell(turret *weapon);
void draw_shell(shell *target, SDL_Renderer *renderer, camera_t *cam);
void shell_destroy(shell *target);

// return true if the shell is in the water
bool is_shell_in_water(shell *bullet);

// draw a shell on the rendereing target
void draw_shell(shell *target, SDL_Renderer *renderer, camera_t *cam);

////////////////////////////////// shell_list //////////////////////////////////

/* "List-array" of shells*. It is meant to be used with the map function. */
typedef struct shell_list {
    // number of shells inside
    int length;

    // size of the array elements; will increase when trying to add new elements
    int max_capacity;

    /* array of pointers to dynamic shells. Relevant data is stored up to
    length - 1 (included) */
    shell **elements;
} shell_list;

/* Create a new empty shell_list.
 * initial_size should be greater than 0. */
shell_list *shell_list_create(int initial_size);

/* Free the list from memory. The shells inside are not freed.*/
void shell_list_destroy(shell_list *list);

/* Return the length of the list. */
int shell_list_length(shell_list *list);

/* Add a shell to the list. */
void shell_list_add_element(shell_list *list, shell *one_shell);

/* Remove all shells that verify `should_be_removed(one_shell)` from the list.
 * These elements are freed from memory and forgotten by the list. */
void shell_list_remove_elements(shell_list *list,
                                bool (*should_be_removed)(shell *one_shell));

/* Return true. Can be used to empty a shell_list. */
bool true_func(shell *s);

/* Apply func to each element of list.
 * func can modify shell, but it should not add shells in the list. */
void shell_list_map(shell_list *list, void (*func)(shell *one_shell));

/* Draw each shell from the list on the render target. */
void draw_shell_list(shell_list *list, SDL_Renderer *renderer, camera_t cam);

/* Test all shells in the list for a collision with target_ship.
 * If a shell from a diferent ship hits it, damage is applied and the shell
 * is freed from the list. */
void apply_shell_collisions(shell_list *list, ship *target_ship);

/////////////////////////////// shell_list (end) ///////////////////////////////

/////////////////////////////////// weapons ///////////////////////////////////

// Direction where an oriented object present on a ship is facing
typedef int on_ship_orientation;
#define TOWARDS_BOW 1    // towards front of the ship
#define TOWARDS_STERN -1 // towards back of the ship

// data needed to create a new turret (see turret structure definition for
// details)
typedef struct {
    int max_health;
    int armor;
    int caliber;
    int power;
    int min_cannon_angle;
    int max_cannon_angle;
    int default_cooldown;

    // geometry / textures
    int body_width, body_height;
    int cannon_width, cannon_height;
    // texture position in sheet
    SDL_Rect body_texture_rect, cannon_texture_rect;
    SDL_Point body_pivot_point;   // relative to top left of body
    SDL_Point cannon_pivot_point; // relative to top left of cannon
} turret_type_data;

// type of turrets
// this is not an enum because it cannot be forward declared
#define TURRET_TYPE_AUTOCANNON 0
typedef int turret_type;

/* Return a turret_type_data that contains all the data needed to initialize a
 * turret. */
turret_type_data default_turret_data(turret_type type);

// Represents a turret during battle
struct turret {

    //////////////////////////////// static data ///////////////////////////////

    // ship the turret is mounted on
    ship *owner_ship;

    int max_health; // maximum turret health
    // Damage reduction, linear between 0 and 9:
    //  0 = 0% protection, 9 = 90% protection
    int armor;

    int caliber;
    int power; // Impacts initial shell speed

    // int burst;          // TODO: documentation
    // int number_cannons; // TODO: documentation

    // shot: kinematics
    int min_cannon_angle;
    int max_cannon_angle;

    // in-between shots waiting time
    int default_cooldown;

    // geometry section

    SDL_Rect relative_position;      // turret position, relative to hull
    on_ship_orientation orientation; // towards bow or stern

    // point of the turret where the cannon is attached and where the shell is
    // spawned, relative to the top left of the turret
    SDL_Point turret_pivot;

    // point of the cannon where the turret is attached,
    // relative to the top left of the cannon
    SDL_Point cannon_pivot;

    // turret graphical data
    SDL_Rect body_texture_rect; // turret body texture position in texture sheet
    // cannon position, relative to turret, with default angle (180 if pointing
    // left, else 0 degrees)
    SDL_Rect cannon_rect;
    SDL_Rect cannon_texture_rect; // cannon texture position in texture sheet

    ////////////////////////////// dynamic data //////////////////////////////

    // data from the currently loaded shell
    shell selected_shell;

    // angle of the cannon in degrees (if facing towards bow:
    // 0 degrees: pointing right, clockwise rotation)
    int cannon_angle;

    // not implemented yet, max value of random spread (effective
    // firing angle = angle + rng(+spread, -spread))
    /*
    float spread;
    float initial_shell_speed;
    */

    int health;
    int time_on_fire; // Remaining time for the weapon to be on fire

    // State of the weapon according to its current health:
    // 0 = destroyed, 2 = full health
    int state;

    // cooldown section

    // current time before the turret can shoot again
    int remaining_cooldown;
};

/** Return a pointer to a newly created and initialized turret.
 @param t_type Type of the turret
 @param relative_coordinate Coordinates of the turret, relative to the top left
 of the hull
 @param owner_ship Pointer to the ship where the turret will be mounted
 @param orientation Orientation of the turret on the ship.
 @param point_left True if the ship is pointing left.
*/
turret *create_turret(turret_type t_type, SDL_Point relative_coordinate,
                      ship *owner_ship, on_ship_orientation orientation,
                      bool point_left);

/* Free heap memory allocated for the turret structure. */
void free_turret(turret *target);

void turret_cooldown(turret *target);

void turret_lost_health(turret *target, shell *incoming);

int test_weapon(int emp_size, int weapon_name);

void turret_fire_damage(turret *target, float tickspeed);

#endif /* WEAPON_H */
