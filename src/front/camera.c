#include "camera.h"
#include "background.h"
#include "user_interface.h"
#include <SDL2/SDL_rect.h>
#include <stdbool.h>

void update_bg_display_pos(background_graphic *bg_graphic, camera_t cam) {

    sea_graphic *sea_1 = &bg_graphic->sea_1;
    sea_graphic *sea_2 = &bg_graphic->sea_2;

    // shift the sea
    sea_1->rect.x = sea_1->x - cam.rect.x;
    sea_2->rect.x = sea_2->x - cam.rect.x;
    // if sea_1 is entirely out of frame (left)
    if (sea_1->rect.x + sea_1->rect.w < 0) {
        sea_1->x = sea_2->x + sea_2->rect.w;
    }
    // if sea_2 is entirely out of frame (left)
    if (sea_2->rect.x + sea_2->rect.w < 0) {
        sea_2->x = sea_1->x + sea_1->rect.w;
    }
    // if sea_1 is entirely out of frame (right)
    if (sea_1->rect.x > WINDOW_WIDTH) {
        sea_1->x = sea_2->x - sea_1->rect.w;
    }
    // if sea_2 is entirely out of frame (right)
    if (sea_2->rect.x > WINDOW_WIDTH) {
        sea_2->x = sea_1->x - sea_2->rect.w;
    }
}

void move_camera_towards_object(SDL_Rect *target, camera_t *cam) {

    int object_center_x = target->x + target->w / 2;
    int target_x = object_center_x - WINDOW_WIDTH / 2;
    int distance = abs(cam->rect.x - target_x);

    // Adjust the divisor to control the speed of the transition
    int transition_speed = distance / 5;

    // if the camera is near enough of its target
    if (distance <= transition_speed || distance <= 5) {
        cam->rect.x = target_x;
    } else if (cam->rect.x < target_x) {
        cam->rect.x += transition_speed;
    } else {
        cam->rect.x -= transition_speed;
    }
}

SDL_Point ingame_coords_to_screen(SDL_Point input, camera_t cam) {
    SDL_Point output;
    output.y = MAIN_VIEW_HEIGHT - SEA_BASE_HEIGHT - input.y - cam.rect.y;
    output.x = input.x - cam.rect.x;
    return (output);
}