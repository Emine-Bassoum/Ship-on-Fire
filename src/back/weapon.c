#include "weapon.h"
#include "../front/camera.h"
#include "assert.h"
#include "geometry.h"
#include "ship.h"
#include <SDL2/SDL_rect.h>
#include <stdlib.h>

/////////////////////////// shell-related functions ///////////////////////////

// Create a shell from a given weapon
shell *create_shell(turret *t) {

    shell *result = malloc(sizeof(shell));
    assert(result != NULL);

    result->accumulated_speed_change = 0.0f;
    result->caliber = t->caliber;
    result->coordinates = shift_point(
        t->turret_pivot, shift_point(rect_coords(t->relative_position),
                                     t->owner_ship->coordinates));

    result->speed.x =
        t->power * cos(deg_to_rad(t->cannon_angle)) + t->owner_ship->speed.x;
    result->speed.y =
        t->power * sin(-deg_to_rad(t->cannon_angle)) + t->owner_ship->speed.y;
    result->owner = t->owner_ship;
    result->power = t->power;

    // special attributes
    result->explosive = t->selected_shell.explosive;
    result->flare = t->selected_shell.flare;
    result->smoke = t->selected_shell.smoke;
    result->piercing = t->selected_shell.piercing;
    result->incendiary = t->selected_shell.incendiary;

    return result;
}

void shell_destroy(shell *target) {
    free(target);
}

void draw_shell(shell *target, SDL_Renderer *renderer, camera_t *cam) {
    SDL_Rect rect = (SDL_Rect){
        .x = target->coordinates.x,
        .y = target->coordinates.y,
        .w = 7,
        .h = 7,
    };
    SDL_Point point = rect_coords(rect);
    point = ingame_coords_to_screen(point, *cam);
    rect.x = point.x;
    rect.y = point.y;
    // Change color to gray/silvery
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &rect);
}

bool is_shell_in_water(shell *bullet) {
    return bullet->coordinates.y < 0;
}
////////////////////////////// shell_list //////////////////////////////

shell_list *shell_list_create(int initial_size) {
    shell_list *list = malloc(sizeof(shell_list));
    assert(list != NULL);

    shell **elements = malloc(initial_size * sizeof(shell *));
    assert(list != NULL);

    list->max_capacity = initial_size;
    list->length = 0;
    list->elements = elements;

    return list;
}

void shell_list_destroy(shell_list *list) {
    free(list->elements);
    free(list);
}

int shell_list_length(shell_list *list) {
    return list->length;
}

void shell_list_add_element(shell_list *list, shell *one_shell) {
    // reallocation only if the array is full
    if (list->length == list->max_capacity) {
        size_t newsize = (list->length + 1) * sizeof(shell *);
        list->elements = realloc(list->elements, newsize);
        assert(list->elements != NULL);
        list->max_capacity++;
    }
    list->elements[list->length] = one_shell;
    list->length++;
    return;
}

void shell_list_remove_elements(shell_list *list,
                                bool (*should_be_removed)(shell *one_shell)) {

    for (int i = list->length - 1; i >= 0; i--) {
        if (should_be_removed(list->elements[i])) {
            free(list->elements[i]);
            list->elements[i] = list->elements[list->length - 1];
            list->length--;
        }
    }
    return;
}

bool true_func(shell *s) {
    (void)s; // Cast s to void to avoid unused variable warning
    return true;
}

void shell_list_map(shell_list *list, void (*func)(shell *one_shell)) {

    for (int i = 0; i < list->length; i++) {
        func(list->elements[i]);
    }
    return;
}

void draw_shell_list(shell_list *list, SDL_Renderer *renderer, camera_t cam) {

    for (int i = 0; i < list->length; i++) {
        draw_shell(list->elements[i], renderer, &cam);
    }
}

void apply_shell_collisions(shell_list *list, ship *target_ship) {

    for (int i = 0; i < list->length; i++) {

        shell *current_shell = list->elements[i];
        if (current_shell->owner != target_ship) { // diferent ship

            SDL_Rect in_game_hitbox =
                shift_rect(target_ship->hitbox, target_ship->coordinates);

            // collision
            if (SDL_PointInRect(&current_shell->coordinates, &in_game_hitbox)) {

                inflict_shell_dmg(target_ship, current_shell);

                // display info
                printf(
                    "Shell impact (%s -> %s)! %d hull HP, %d crew HP.\n",
                    current_shell->owner->owner_player->name,
                    target_ship->owner_player->name, target_ship->hull_health,
                    target_ship->crew_health);

                // remove shell from list
                free(current_shell);
                list->elements[i] = list->elements[list->length - 1];
                list->length--;
            }
        }
    }
}

///////////////////////// turret-related functions ////////////////////////////

turret_type_data default_turret_data(turret_type type) {

    turret_type_data res; // result

    // IMPORTANT if adding new turret types:
    // width, heigth and pivot points: real texture size * SHIP_SCALE;
    // pivot points: exact center of point (so add 0.5 if it is 1 px wide)
    switch (type) {

    case TURRET_TYPE_AUTOCANNON:

        res.max_health = 100;
        res.armor = 3;
        res.caliber = 20;
        res.power = 10;
        res.min_cannon_angle = -80;
        res.max_cannon_angle = 10;
        res.default_cooldown = 4;

        res.body_width = 12 * SHIP_SCALE;
        res.body_height = 5 * SHIP_SCALE;
        res.cannon_width = 12 * SHIP_SCALE;
        res.cannon_height = 3 * SHIP_SCALE;

        res.body_texture_rect = (SDL_Rect){
            .x = 0,
            .y = 0,
            .w = 12,
            .h = 5,
        };
        res.cannon_texture_rect = (SDL_Rect){
            .x = 32 * 3, // 4th sprite, 32 px wide
            .y = 0,
            .w = 12,
            .h = 3,
        };
        res.body_pivot_point = (SDL_Point){
            (11 + 0.5) * SHIP_SCALE,
            (3 - 2 + 0.5) * SHIP_SCALE, // -2: special padding
        };
        res.cannon_pivot_point = (SDL_Point){
            (1 + 0.5) * SHIP_SCALE,
            (1 + 0.5) * SHIP_SCALE,
        };
        break;
    }

    return res;
}

turret *create_turret(turret_type t_type, SDL_Point relative_coordinate,
                      ship *owner_ship, on_ship_orientation orientation,
                      bool point_left) {

    turret *res = malloc(sizeof(turret));                  // result
    turret_type_data t_data = default_turret_data(t_type); // turret data to use

    res->owner_ship = owner_ship;

    res->max_health = t_data.max_health;
    res->armor = t_data.armor;

    res->caliber = t_data.caliber;
    res->power = t_data.power;

    if (point_left) { // if ship is flipped (pointing towards left)

        // flip & shift turret body
        res->relative_position = (SDL_Rect){
            .x = owner_ship->width - relative_coordinate.x - t_data.body_width,
            .y = relative_coordinate.y,
            .w = t_data.body_width,
            .h = t_data.body_height,
        };
        // angle
        res->cannon_angle = 180;
        res->max_cannon_angle = 180 - t_data.min_cannon_angle;
        res->min_cannon_angle = 180 - t_data.max_cannon_angle;

        res->turret_pivot = (SDL_Point){
            .x = -abs(t_data.body_pivot_point.x - t_data.body_width) +
                 1 * SHIP_SCALE, // 1 * SHIP_SCALE: special flipped padding
            .y = t_data.body_pivot_point.y};

    } else { // ship points towards right

        res->relative_position = (SDL_Rect){
            .x = relative_coordinate.x,
            .y = relative_coordinate.y,
            .w = t_data.body_width,
            .h = t_data.body_height,
        };

        res->cannon_angle = 0;
        res->min_cannon_angle = t_data.min_cannon_angle;
        res->max_cannon_angle = t_data.max_cannon_angle;

        res->turret_pivot = t_data.body_pivot_point;
    }

    res->cannon_rect = (SDL_Rect){
        .x = res->turret_pivot.x - t_data.cannon_pivot_point.x,
        .y = -(res->turret_pivot.y - t_data.cannon_pivot_point.y),
        .w = t_data.cannon_width,
        .h = t_data.cannon_height,
    };

    res->orientation = orientation;

    res->cannon_pivot = t_data.cannon_pivot_point;

    res->body_texture_rect = t_data.body_texture_rect;

    res->cannon_texture_rect = t_data.cannon_texture_rect;

    res->default_cooldown = t_data.default_cooldown;

    ////////////////////////////// dynamic data
    /////////////////////////////////

    res->selected_shell = (shell){
        .caliber = 1,
        .power = 1,
        .explosive = false,
        .flare = false,
        .smoke = false,
        .piercing = false,
        .incendiary = false,
        // coordinates or speed are not initialized
    };

    res->health = t_data.max_health;
    res->time_on_fire = 0;

    res->state = 2;

    res->remaining_cooldown = 0;

    return res;
}

void free_turret(turret *target) {
    free(target);
}

/* Calculates the effective cooldown from the crew onboard */
void turret_cooldown(turret *target) {
    ship *ship = target->owner_ship;

    float crew_health_ratio =
        (float)(ship->crew_health) / (float)(ship->max_crew_health);

    if (crew_health_ratio > 0.9) {

        target->default_cooldown = target->default_cooldown;

    } else if (crew_health_ratio > 0.2) {

        target->default_cooldown =
            target->default_cooldown / (0.714 * crew_health_ratio + 0.3572);

    } else { // ratio <= 0.2

        target->default_cooldown = target->default_cooldown / 0.5;
    }
}

void turret_lost_health(turret *target, shell *incoming) {
    if (target->state != 0) {
        int damage = 0.5 * incoming->caliber;
        int penetration;

        if (incoming->explosive) {
            penetration = 1 * (int)incoming->caliber / 100;
        }

        if (incoming->flare) {
            penetration = 0;
            damage = 1;
        }

        if (incoming->smoke) {
            penetration = 0;
            damage = 1;
        }

        if (incoming->piercing) {
            penetration = 4 * (int)incoming->caliber / 100;
        }

        if (incoming->incendiary) {
            penetration = 0;
            damage *= 0.5;
            target->time_on_fire += 0.5 * incoming->caliber;
        }

        if (target->health < penetration) {
            if (target->health - damage < 0) {
                target->state = 0;
                target->health = 0;
            } else {
                target->health -= damage;
                if (target->health < target->max_health / 2) {
                    target->state = 1;
                }
            }
        } else {
            if (target->health - damage * (target->health - penetration) * 0.1 <
                0) {
                target->state = 0;
                target->health = 0;
            } else {
                target->health -= damage * (target->health - penetration) * 0.1;
                if (target->health < target->max_health / 2) {
                    target->state = 1;
                }
            }
        }
    }
}

void turret_fire_damage(turret *target, float tickspeed) {

    if (target->time_on_fire > 0) {
        if (target->health - 0.5 <= 0) {
            target->health = 0;
            target->state = 0;
        } else {
            target->health -= 0.5;
            if (target->health < target->max_health / 2) {
                target->state = 1;
            }
        }
        if (target->time_on_fire - tickspeed < 0) {
            target->time_on_fire = 0;
        } else {
            target->time_on_fire -= tickspeed;
        }
    }
}
