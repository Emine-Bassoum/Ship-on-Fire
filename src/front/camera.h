#ifndef CAMERA_H
#define CAMERA_H

#include "background.h"
#include <stdbool.h>

#define BORDER_DISTANCE 400

typedef enum {
    CAMERA_MODE_FREE,  // camera can be moved freely
    CAMERA_MODE_FIXED, // camera is fixed on a particular object
} camera_mode;

typedef struct {
    SDL_Rect rect;    // view of the game
    camera_mode mode; // currrent mode the camera is in
    int ship_target;  // number of the targeted ship (1 or 2) 
} camera_t;

/* Update the background graphical elements on-screen position,
 * according to the settings of the camera. */
void update_bg_display_pos(background_graphic *bg_graphic, camera_t cam);

/**
 * Move the camera frame towards an object fitting in a rectangle.
 * @param target Real dimensions of the object. */
void move_camera_towards_object(SDL_Rect *target, camera_t *cam);

/* Translate the in-game coordinates to on-screen coordinates. */
SDL_Point ingame_coords_to_screen(SDL_Point input, camera_t cam);

#endif
