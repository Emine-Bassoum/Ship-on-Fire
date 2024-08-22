/* UI-related structures and functions (including graphical functions)
 * are declared here. */

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "../back/command.h"
#include "../back/ship.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

// resolution of the window
#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

////////////////////////////////// interface //////////////////////////////////

/* Definitions:
 *
 * top bar: horizontal bar
 * main view: central view of the ships
 * bottom bar: horizontal bar containing:
 *     - 4 cannon buttons (centered)
 *     - the life bars (right)
 * bottom left: chadburn:
 *     - body
 *     - selector
 *
 * Textures: always use the real sizes of the textures:
 * displayed size = texture size * scale factor
 */

////////////////////// proportions of the interface parts //////////////////////

// distance between the top of window and the bottom bar
#define BOTTOM_BAR_PADDING ((3 * WINDOW_HEIGHT) / 4)
#define BOTTOM_BAR_HEIGHT (WINDOW_HEIGHT / 4)

// no top bar yet
#define TOP_BAR_HEIGHT 0

// height of the central view on the ships
#define MAIN_VIEW_HEIGHT (WINDOW_HEIGHT - TOP_BAR_HEIGHT - BOTTOM_BAR_HEIGHT)

//////////////////////////// card buttons dimensions ///////////////////////////

// number of buttons in the interface
#define BUTTON_CHADBURN_NUMBER 2
#define BUTTON_CANNON_NUMBER 1

// total number of buttons
#define BUTTON_NUMBER (BUTTON_CHADBURN_NUMBER + BUTTON_CANNON_NUMBER)

// scaling factor for cards texture
#define SCALE_CARDS 3

#define CARD_BUTTON_WIDTH (96 * SCALE_CARDS)
#define CARD_BUTTON_HEIGHT (48 * SCALE_CARDS)

#define X_DISTANCE_BETWEEN_CARDS 10

// x-axis of the start of the zone containing cannon cards (centered)
#define CARDS_BUTTON_START_X                                                   \
    ((WINDOW_WIDTH / 2) -                                                      \
     (BUTTON_CANNON_NUMBER * CARD_BUTTON_WIDTH +                               \
      (BUTTON_CANNON_NUMBER - 1) * X_DISTANCE_BETWEEN_CARDS) /                 \
         2)

// all the types of buttons that can be used in the interface
typedef enum {
    BUTTON_TYPE_CHADBURN_DECREASE,
    BUTTON_TYPE_CHADBURN_INCREASE,
    BUTTON_TYPE_CANNON_1,
    BUTTON_TYPE_CANNON_2,
    BUTTON_TYPE_PAUSE_MENU
} button_type;

// states a button can be in ;
typedef enum {
    BUTTON_STATE_NORMAL,      // displayed
    BUTTON_STATE_UNCLICKABLE, // displayed and not clickable
    BUTTON_STATE_HIDDEN,      // not displayed and not clickable

} button_state;

// default cooldown (in frames)
#define BUTTON_DEFAULT_CLICK_COOLDOWN 15

/* Representation of a clickable area of the UI that has a texture. */
typedef struct {

    button_type type;   // Type of button
    button_state state; // Whether button should be displayed and interactable
    int click_cooldown; // Number of frames before button can be clicked again

    SDL_Rect rect;          // Absolute position in interface
    SDL_Texture *texture;   // Texture sheet containing button textures
    SDL_Rect *texture_rect; // Position and size of the texture in texture sheet

} interface_button;

/* Create a new interface_button and initialize it based on type. */
interface_button *create_button(int x, int y, button_type type,
                                SDL_Renderer *renderer);

/* Return an array containing all clickable buttons */
interface_button **create_all_buttons(SDL_Renderer *renderer);

extern interface_button **all_buttons;

// Update the cooldown values of the buttons, and reset texture if needed.
void button_cooldown(interface_button **all_buttons);

/* Free all the buttons of the interface. */
void free_all_buttons(interface_button **all_buttons);

/////////////////////////////////// Chadburn ///////////////////////////////////

// scaling factor for chadburn-related on-screen objects
#define SCALE_CHADBURN 2.5

// On-screen dimensions of the chadburn buttons, body and selector textures
#define CHADBURN_BUTTON_WIDTH (48 * SCALE_CHADBURN)
#define CHADBURN_BUTTON_HEIGHT (48 * SCALE_CHADBURN)
#define CHADBURN_BODY_WIDTH (128 * SCALE_CHADBURN)
#define CHADBURN_BODY_HEIGHT (128 * SCALE_CHADBURN)
#define CHADBURN_SELECTOR_WIDTH (128 * SCALE_CHADBURN)
#define CHADBURN_SELECTOR_HEIGHT (64 * SCALE_CHADBURN)

// Distance between chadburn body x and button x
#define CHADBURN_X_BUTTON_PADDING (8 * SCALE_CHADBURN)
// Distance between chadburn body y and button y
#define CHADBURN_Y_BUTTON_PADDING (72 * SCALE_CHADBURN)

/* Graphical data of the chadburn. Two parts are displayed: the body
 * (doesn't change) and the selector (can change according to speed chosen). */
typedef struct {

    // Absolute body position in interface
    SDL_Rect body_rect;
    // Absolute selector position in interface
    SDL_Rect selector_rect;

    // Texture sheet containing button textures
    SDL_Texture *body_texture;
    SDL_Texture *selector_texture;

    // Position and size of the selector texture in texture sheet
    SDL_Rect *selector_texture_rect;

} chadburn_graphic;

/* Initialize the chadburn graphic and put it in neutral position. */
void init_chadburn_graphic(chadburn_graphic *mychadburn_graphic,
                           SDL_Renderer *renderer);

/* Free heap memory allocated for the chadburn graphic. */
void free_chadburn_graphic(chadburn_graphic graphic);

/* Draw the chadburn on the screen.
 * The selectorwill be in the position stored in the graphic. */
void draw_chadburn_graphic(SDL_Renderer *renderer, chadburn_graphic *graphic);

void set_speed_chadburn_graphic(chadburn_graphic *graphic,
                                chadburn_speed speed);

// texture at the back of bottom bar
typedef struct {
    SDL_Rect rect;
    SDL_Texture *texture;
} ui_bg_graphic;

void init_ui_bg_graphic(ui_bg_graphic *bg, SDL_Renderer *renderer);

void draw_ui_bg(SDL_Renderer *renderer, ui_bg_graphic *bg);

void free_ui_bg_graphic(ui_bg_graphic bg);

#endif /* USER_INTERFACE_H */