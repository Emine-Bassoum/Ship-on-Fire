/* Background-related structures and functions are defined here. */

#ifndef SEA_GRAPHIC_H
#define SEA_GRAPHIC_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>

// visual height of the sea
#define GRAPHICAL_SEA_HEIGHT 200
// distance between bottom of level view and line of in-game coordinate x = 0 
#define SEA_BASE_HEIGHT 100

typedef struct {
    int x; // real coordinate
    SDL_Rect rect;
    SDL_Texture *texture;
} sea_graphic;

typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
} sun;

typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
} sky;

typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
    int speed; // speed the cloud will travel in the sky (pixel/frame)
} cloud;

/* Structure that contains all data needed to display the background.
 * Elements named name_1, name_2 exist so that they can scroll through the
 * screen. */
typedef struct {
    sea_graphic sea_1, sea_2;
    sun sun;
    sky sky;
    cloud cloud1_1, cloud1_2, cloud2_1, cloud2_2;
} background_graphic;

/**
 * Initialize a sea_graphic
 * @param sea_graphic Pointer to the sea_graphic structure
 * @param x Initial x-coordinate
 * @param y Initial y-coordinate
 * @param w Width of the sea_graphic
 * @param h Height of the sea_graphic
 */
void init_sea_graphic(SDL_Renderer *renderer, sea_graphic *sea, int x, int y,
                      int w, int h);

/**
 * Draw the sea_graphic.
 * @param renderer The renderer to draw the sea_graphic on
 * @param sea_graphic The sea_graphic to draw
 */
void draw_sea_graphic(SDL_Renderer *renderer, sea_graphic *ship_graphic);

/* Initialize the sun graphic. */
void init_sun(SDL_Renderer *renderer, sun *sun);

/**
 * Draw the sun.
 * @param renderer The renderer to draw the sun on
 * @param sun The sun to draw
 */
void draw_sun(SDL_Renderer *renderer, sun *sun);

/* Initialize the sky graphic. */
void init_sky(SDL_Renderer *renderer, sky *sky);

/**
 * Draw the sky.
 * @param renderer The renderer to draw the sky on
 * @param sky The sky to draw
 */
void draw_sky(SDL_Renderer *renderer, sky *sky);

/* Initialize a cloud graphic. */
void init_cloud(SDL_Renderer *renderer, cloud *cloud, int cloud_number,
                int speed);

/* Update a cloud's postion and draw it. */
void move_and_draw_cloud(SDL_Renderer *renderer, cloud *cloud);

/* Initialize all background elements. */
void init_background(SDL_Renderer *renderer, background_graphic *bg);

/* Draw the whole background on screen. */
void draw_background(SDL_Renderer *renderer, background_graphic *bg);

/* Free all background graphic elements from memory. */
void free_background(background_graphic *bg);

#endif // SEA_GRAPHIC_H