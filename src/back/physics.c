/* Functions related to the physical game engine are defined here.*/

#include "physics.h"
#include "geometry.h"
#include "ship.h"
#include <SDL2/SDL_rect.h>
#include <math.h>
#include <stdio.h>

#define ENERGY_TRANSMISSION_RATIO 0.75

void update_ship_kinematics(ship *ship) {
    set_ship_target_speed(ship); // set target speed

    float speed_difference = ship->target_speed - ship->speed.x;
    // Change speed by a small amount
    float speed_change = fmin(fabs(speed_difference), 0.1);

    // printf("speed_difference: %f, ship_target: %d, ship_speed: %d\n",
    //        speed_difference, ship->target_speed, ship->speed.x);

    if (speed_difference > 0) {
        ship->accumulated_speed_change +=
            speed_change; // Increase accumulated speed change
        if (ship->accumulated_speed_change >= 1.0f) {
            ship->speed.x += 1; // Increase speed
            ship->accumulated_speed_change -=
                1.0f; // Subtract the change we've applied
        }
    } else if (speed_difference < 0) {
        ship->accumulated_speed_change +=
            speed_change; // Increase accumulated speed change
        if (ship->accumulated_speed_change >= 1.0f) {
            ship->speed.x -= 1; // Decrease speed
            ship->accumulated_speed_change -=
                1.0f; // Subtract the change we've applied
        }
    }
    ship->coordinates.x += ship->speed.x; // Move ship
}

void update_shell_kinematics(shell *s) {
    const float GRAVITY = 0.05f; // Define gravity constant

    // Accumulate speed change due to gravity
    s->accumulated_speed_change -= GRAVITY;

    // If accumulated speed change is greater than or equal to 1
    if (s->accumulated_speed_change <= -1.0f) {
        // Increase vertical speed
        s->speed.y += s->accumulated_speed_change;
        // Reset accumulated speed change
        s->accumulated_speed_change = 0;
    }

    // Update position based on speed
    s->coordinates.x += s->speed.x;
    s->coordinates.y += s->speed.y;

    // printf("shell_speed_x: %d, shell_speed_y: %d\n", s->speed.x, s->speed.y);
}

void set_ship_target_speed(ship *ship) {

    // orient the speed in the right direction
    int reverse_if_left;
    if (ship->point_left) {
        reverse_if_left = -1;
    } else {
        reverse_if_left = 1;
    }

    switch (ship->chadburn_setting) {

    case CHADBURN_SPEED_FULL_REVERSE:
        ship->target_speed = -reverse_if_left * ship->max_rev_speed;
        break;

    case CHADBURN_SPEED_HALF_REVERSE:
        ship->target_speed = -reverse_if_left * 0.5 * ship->max_rev_speed;
        break;

    case CHADBURN_SPEED_NEUTRAL:
        ship->target_speed = 0;
        break;

    case CHADBURN_SPEED_THIRD_AHEAD:
        ship->target_speed = reverse_if_left * 0.33 * ship->max_speed;
        break;

    case CHADBURN_SPEED_TWO_THIRDS_AHEAD:
        ship->target_speed = reverse_if_left * 0.66 * ship->max_speed;
        break;

    case CHADBURN_SPEED_FULL_AHEAD:
        ship->target_speed = reverse_if_left * ship->max_speed;
        break;

    default:
        fprintf(stderr, "set_ship_target_speed: wrong chadburn setting\n");
        exit(EXIT_FAILURE);
    }
}

void apply_ship_on_ship_collisions(ship *ship_1, ship *ship_2) {

    // in-game hitboxes
    SDL_Rect realbox1 = shift_rect(ship_1->hitbox, ship_1->coordinates);
    SDL_Rect realbox2 = shift_rect(ship_2->hitbox, ship_2->coordinates);

    SDL_Rect box_intersection; // intersection of the two hitboxes

    // ship-ship collision
    if (SDL_IntersectRect(&realbox1, &realbox2, &box_intersection)) {

        // move ships out of each other:

        // if ship_1 on left of ship_2
        if (ship_1->coordinates.x <= ship_2->coordinates.x) {

            ship_1->coordinates.x -= box_intersection.w / 2;
            ship_2->coordinates.x += box_intersection.w / 2;
        } else {

            ship_1->coordinates.x += box_intersection.w / 2;
            ship_2->coordinates.x -= box_intersection.w / 2;
        }

        // TODO: same on y axis

        float m1 = ship_1->weight;
        float m2 = ship_2->weight;
        int mtot = m1 + m2; // total mass

        // initial speeds
        SDL_Point v1 = ship_1->speed;
        SDL_Point v2 = ship_2->speed;

        SDL_Point new_v1, new_v2; // new speeds

        // new speeds calclated using momentum conservation equations
        new_v1 = (SDL_Point){
            .x = ENERGY_TRANSMISSION_RATIO *
                 ((v1.x * (m1 - m2)) + (2 * m2 * v2.x)) / mtot,
            .y = ENERGY_TRANSMISSION_RATIO *
                 ((v1.y * (m1 - m2)) + (2 * m2 * v2.y)) / mtot,
        };
        new_v2 = (SDL_Point){
            .x = ENERGY_TRANSMISSION_RATIO *
                 ((v2.x * (m2 - m1)) + (2 * m1 * v1.x)) / mtot,
            .y = ENERGY_TRANSMISSION_RATIO *
                 ((v2.y * (m2 - m1)) + (2 * m1 * v1.y)) / mtot,
        };

        // update sheep speed including energy loss
        ship_1->speed = new_v1;
        ship_2->speed = new_v2;

        // remove health
        // damage inflicted proportionnal to speed difference
        int dmg = 5 * abs(v1.x - v2.x);

        // don't apply damage for small values
        if (dmg > 5) {
            {
                printf("Ship impact! %d dmg inflicted. "
                       "Hull HP left: %s: %d HP, %s: %d HP.\n",
                       dmg, ship_1->owner_player->name, ship_1->hull_health,
                       ship_2->owner_player->name, ship_2->hull_health);
                ship_1->hull_health -= dmg;
                ship_2->hull_health -= dmg;
            }
        }
        return;
    }
}