#include "ship.h"
#include "../front/common_graphic.h"
#include "geometry.h"
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

ship_type_data default_ship_data(ship_type type) {

    ship_type_data res; // result

    // IMPORTANT if you are adding new types:
    // For width, heigth and turret coords: real texture size * SHIP_SCALE
    switch (type) {

    case SHIP_TYPE_CORVETTE:

        res.max_hull_health = 1000;
        res.max_crew_health = 1000;
        res.armor = 3;

        res.max_speed = 35;
        res.max_rev_speed = 20;
        res.weight = 100;

        res.turret_number = 1;
        res.turrets_type = malloc(1 * sizeof(turret_type));
        assert(res.turrets_type != NULL);
        res.turrets_type[0] = TURRET_TYPE_AUTOCANNON;

        res.turret_coord = malloc(1 * sizeof(SDL_Point));
        assert(res.turret_coord != NULL);
        res.turret_coord[0] = (SDL_Point){87 * SHIP_SCALE, 29 * SHIP_SCALE};

        res.turret_orientation = malloc(1 * sizeof(on_ship_orientation));
        assert(res.turret_orientation != NULL);
        res.turret_orientation[0] = TOWARDS_BOW;

        res.width = 128 * SHIP_SCALE;
        res.height = 64 * SHIP_SCALE;
        res.hull_texture_rect = (SDL_Rect){.x = 0, .y = 0, .w = 128, .h = 64};

        res.hitbox = (SDL_Rect){0, 0, res.width, res.height / 2};

        break;

    case SHIP_TYPE_1:
    case SHIP_TYPE_2:
    case SHIP_TYPE_3:
    case SHIP_TYPE_4:
        fprintf(stderr,
                "Error in default_ship_data: ship type not implemented yet");
        break;
    }
    return res;
}

void free_ship_type_data(ship_type_data data) {
    free(data.turrets_type);
    free(data.turret_coord);
    free(data.turret_orientation);
    return;
}

ship *create_new_ship(player_t *owner, ship_type s_type, SDL_Point coordinates,
                      SDL_Renderer *renderer, bool point_left) {

    ship_type_data s_data = default_ship_data(s_type);

    ship *res = malloc(sizeof(ship)); // result
    assert(res != NULL);

    /////////////////////////// static ship data ///////////////////////////////

    res->owner_player = owner;

    res->max_hull_health = s_data.max_hull_health;
    res->max_crew_health = s_data.max_crew_health;

    res->max_speed = s_data.max_speed;
    res->max_rev_speed = s_data.max_rev_speed;
    res->max_acc = s_data.max_acc;
    res->weight = s_data.weight;

    res->turret_number = s_data.turret_number;

    if (point_left) {
        // flip hitbox
        res->hitbox = (SDL_Rect){
            .x = s_data.width - s_data.hitbox.x - s_data.hitbox.w,
            .y = s_data.hitbox.y,
            .w = s_data.hitbox.w,
            .h = s_data.hitbox.h,
        };
    } else {
        res->hitbox = s_data.hitbox;
    }

    res->width = s_data.width;
    res->height = s_data.height;

    res->point_left = point_left;

    // ship texture loading
    image_to_texture("../images/ship_sheet.png", &res->hull_texture, NULL, NULL,
                     renderer);

    res->hull_texture_rect = s_data.hull_texture_rect;

    ////////////////////// (static & dynamic) turret data //////////////////////

    res->turret_number = s_data.turret_number;

    if (s_data.turret_number > 0) {
        // turret texture loading
        image_to_texture("../images/turret_sheet.png", &res->turret_texture,
                         NULL, NULL, renderer);

        res->turrets = malloc(s_data.turret_number * sizeof(turret));
        assert(res->turrets != NULL);

        // turret data loading
        for (int i = 0; i < s_data.turret_number; i++) {

            turret_type t_type = s_data.turrets_type[i]; // turret type

            // turret coordinates, relative to top left of ship
            SDL_Point t_coords = s_data.turret_coord[i];

            // turret orientation
            on_ship_orientation t_orient = s_data.turret_orientation[i];

            res->turrets[i] =
                create_turret(t_type, t_coords, res, t_orient, point_left);
        }

    } else { // no turrets on ship
        res->turret_texture = NULL;
        res->turrets = NULL;
    }

    /////////////////////////////// dynamic data ///////////////////////////////

    // kinematic data
    res->coordinates = coordinates;
    res->speed = (SDL_Point){0, 0};
    res->acc = (SDL_Point){0, 0};

    res->chadburn_setting = CHADBURN_SPEED_NEUTRAL;
    res->target_speed = 0;

    // health data
    res->hull_health = s_data.max_hull_health;
    res->crew_health = s_data.max_crew_health;
    res->time_on_fire = 0;
    res->armor = s_data.armor;

    res->state = 2; // TODO : put properly named values

    free_ship_type_data(s_data);
    return res;
}

void free_ship(ship *target) {

    SDL_DestroyTexture(target->hull_texture);
    SDL_DestroyTexture(target->turret_texture);
    for (int i = 0; i < target->turret_number; i++) {
        free_turret(target->turrets[i]);
    }
    free(target->turrets);
    free(target);
    return;
}

void draw_ship(ship *ship_to_draw, SDL_Renderer *renderer, camera_t cam) {
    // Render the hull texture

    // on-screen ship coordinates
    SDL_Point ship_screen_coords =
        ingame_coords_to_screen(ship_to_draw->coordinates, cam);

    // on-screen destination
    SDL_Rect dstrect = (SDL_Rect){
        .x = ship_screen_coords.x,
        .y = ship_screen_coords.y - ship_to_draw->height,
        .w = ship_to_draw->width,
        .h = ship_to_draw->height,
    };

    SDL_Rect srcrect = ship_to_draw->hull_texture_rect;

    // To render a rectangle instead of the texture:

    // SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
    // SDL_RenderFillRect(renderer, &dstrect);

    SDL_RendererFlip should_be_flipped;
    // if pointing_left is true, the turret should be reversed
    if (ship_to_draw->point_left) {
        should_be_flipped = SDL_FLIP_HORIZONTAL;
    } else {
        should_be_flipped = SDL_FLIP_NONE;
    }

    // draw texture
    if (SDL_RenderCopyEx(renderer, ship_to_draw->hull_texture, &srcrect,
                         &dstrect, 0, NULL, should_be_flipped)) {
        fprintf(stderr,
                "draw_ship error: could not render hull. SDL_Error: %s\n",
                SDL_GetError());
        exit(EXIT_FAILURE);
    }

    // Render all turrets
    for (int i = 0; i < ship_to_draw->turret_number; i++) {

        turret turret_to_draw = *ship_to_draw->turrets[i];

        // shift the turret body by the coordinates of the hull
        SDL_Point relative_body_coords = {turret_to_draw.relative_position.x,
                                          turret_to_draw.relative_position.y};

        /*      // if pointing_left is true, the turret is on the left side of
           the ship if (ship_to_draw->point_left) relative_body_coords.x =
           ship_to_draw->width - turret_to_draw.relative_position.x -
                                             turret_to_draw.body_texture_rect.w
           * 2; */

        SDL_Point in_game_body_coords =
            shift_point(relative_body_coords, ship_to_draw->coordinates);

        /*         // if pointing_left is true, the cannon is on the left side
           of the ship if (ship_to_draw->point_left) in_game_cannon_position.x =
                        in_game_cannon_position.x - in_game_cannon_position.w;
         */

        // conversion to on-screen position
        SDL_Point on_screen_body_coords =
            ingame_coords_to_screen(in_game_body_coords, cam);
        SDL_Rect body_dest = {
            .x = on_screen_body_coords.x,
            .y = on_screen_body_coords.y - turret_to_draw.relative_position.h,
            .w = turret_to_draw.relative_position.w,
            .h = turret_to_draw.relative_position.h,
        };

        // shift the cannon by the coordinates of the turret body

        // in-game rectangle representing absolute cannon position
        SDL_Rect in_game_cannon_position =
            shift_rect(turret_to_draw.cannon_rect, in_game_body_coords);

        // on-screen cannon coordinates
        SDL_Point on_screen_cannon_coords =
            ingame_coords_to_screen(rect_coords(in_game_cannon_position), cam);

        // on-screen rectangle that is filled in with the cannon texture
        SDL_Rect cannon_dest = {
            .x = on_screen_cannon_coords.x,
            .y = on_screen_cannon_coords.y - in_game_cannon_position.h,
            .w = in_game_cannon_position.w,
            .h = in_game_cannon_position.h,
        };

        // Render cannon
        if (SDL_RenderCopyEx(renderer, ship_to_draw->turret_texture,
                             &turret_to_draw.cannon_texture_rect, &cannon_dest,
                             turret_to_draw.cannon_angle,
                             &turret_to_draw.cannon_pivot, SDL_FLIP_NONE)) {

            fprintf(stderr,
                    "draw_ship error: could not render cannon. SDL_Error: %s\n",
                    SDL_GetError());
            exit(EXIT_FAILURE);
        }

        // Render turret body
        if (SDL_RenderCopyEx(renderer, ship_to_draw->turret_texture,
                             &turret_to_draw.body_texture_rect, &body_dest, 0,
                             NULL, should_be_flipped)) {

            fprintf(stderr,
                    "draw_ship error: could not render turret body. SDL_Error: "
                    "%s\n",
                    SDL_GetError());
            exit(EXIT_FAILURE);
        }
    }
    return;
}

void draw_ship_hitbox(ship *s, SDL_Renderer *renderer, camera_t camera) {
    SDL_Rect in_game_hitbox_rect = shift_rect(s->hitbox, s->coordinates);
    SDL_Point on_screen_hitbox_coords =
        ingame_coords_to_screen(rect_coords(in_game_hitbox_rect), camera);
    SDL_Rect hitboxrect = {
        on_screen_hitbox_coords.x,
        on_screen_hitbox_coords.y - in_game_hitbox_rect.h,
        in_game_hitbox_rect.w,
        in_game_hitbox_rect.h,
    };
    SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderDrawRect(renderer, &hitboxrect);
}

void inflict_shell_dmg(ship *target, shell *incoming) {

    float damage = 0.5 * incoming->caliber;
    float damage_crew = 0.3 * incoming->caliber;
    int penetration;

    if (incoming->explosive) {
        penetration = 1 * (int)incoming->caliber / 100;
        damage *= 1.2;
        damage_crew *= 1.5;
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
        damage *= 0.9;
        damage_crew *= 0.5;
    }
    if (incoming->incendiary) {
        penetration = 0;
        damage *= 0.5;
        target->time_on_fire += 0.5 * incoming->caliber;
    }

    if (target->hull_health < penetration) {
        if (target->hull_health - damage < 0) {
            target->state = 0;
            target->hull_health = 0;
        } else {
            target->hull_health -= damage;
            if (target->hull_health < target->max_hull_health / 2) {
                target->state = 1;
            }
        }
    } else {
        if (target->hull_health <
            damage * (target->hull_health - penetration) * 0.1) {
            target->state = 0;
            target->hull_health = 0;
        } else {
            target->hull_health -=
                damage * (target->hull_health - penetration) * 0.1;
            if (target->hull_health < target->max_hull_health / 2) {
                target->state = 1;
            }
        }
    }

    if (target->crew_health < damage_crew) {
        target->crew_health = 0;
    } else {
        target->crew_health -= damage_crew;
    }
    return;
}

void inflict_fire_dmg(ship *target, float tickspeed) {
    if (target->time_on_fire > 0) {
        if (target->hull_health - 0.5 <= 0) {
            target->hull_health = 0;
            target->state = 0;
        } else {
            target->hull_health -= 0.5;
            if (target->hull_health < target->max_hull_health / 2) {
                target->state = 1;
            }
        }

        if (target->crew_health - 1 <= 0) {
            target->crew_health = 0;
        } else {
            target->crew_health -= 1;
        }

        if (target->time_on_fire - tickspeed < 0) {
            target->time_on_fire = 0;
        } else {
            target->time_on_fire -= tickspeed;
        }
    }
}
