#include "user_interface.h"
#include "../back/ship.h"
#include "common_graphic.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

interface_button *create_button(int x, int y, button_type type,
                                SDL_Renderer *renderer) {

    interface_button *res = malloc(sizeof(interface_button));
    assert(res != NULL);

    res->type = type;
    res->state = BUTTON_STATE_NORMAL;
    res->click_cooldown = 0;

    res->rect.x = x;
    res->rect.y = y;

    SDL_Rect *texture_rect = malloc(sizeof(SDL_Rect));
    assert(texture_rect != NULL);

    // dimensions of full spritesheet
    int full_texture_width, full_texture_height;

    switch (type) {

    case BUTTON_TYPE_CHADBURN_DECREASE:

        res->rect.w = CHADBURN_BUTTON_WIDTH;
        res->rect.h = CHADBURN_BUTTON_HEIGHT;

        image_to_texture("../images/chadburn_buttons_sheet.png",
                         &(res->texture), &full_texture_width,
                         &full_texture_height, renderer);

        texture_rect->x = 0;
        texture_rect->y = 0;
        // 2 sub-categories of chadburn buttons
        texture_rect->h = full_texture_height / 2;
        texture_rect->w = full_texture_width / 3; // 3 graphical states

        break;

    case BUTTON_TYPE_CHADBURN_INCREASE:

        res->rect.w = CHADBURN_BUTTON_WIDTH;
        res->rect.h = CHADBURN_BUTTON_HEIGHT;

        image_to_texture("../images/chadburn_buttons_sheet.png",
                         &(res->texture), &full_texture_width,
                         &full_texture_height, renderer);

        texture_rect->x = 0;
        texture_rect->y = full_texture_height / 2; // 2d line

        // 2 sub-categories of chadburn buttons
        texture_rect->h = full_texture_height / 2;
        texture_rect->w = full_texture_width / 3; // 3 graphical states

        break;

    case BUTTON_TYPE_CANNON_1:

        res->rect.w = CARD_BUTTON_WIDTH;
        res->rect.h = CARD_BUTTON_HEIGHT;

        image_to_texture("../images/cannon_buttons_sheet.png", &(res->texture),
                         &full_texture_width, &full_texture_height, renderer);

        texture_rect->x = 0;
        texture_rect->y = 0;
        // 2 sub-categories of cannon buttons
        texture_rect->h = full_texture_height / 2;
        texture_rect->w = full_texture_width / 3; // 3 graphical states
        break;

    case BUTTON_TYPE_CANNON_2:

        res->rect.w = CARD_BUTTON_WIDTH;
        res->rect.h = CARD_BUTTON_HEIGHT;

        image_to_texture("../images/cannon_buttons_sheet.png", &(res->texture),
                         &full_texture_width, &full_texture_height, renderer);

        texture_rect->x = 0;
        texture_rect->y = full_texture_height / 2; // 2d line
        // 2 sub-categories of cannon buttons
        texture_rect->h = full_texture_height / 2;
        texture_rect->w = full_texture_width / 3; // 3 graphical states

        break;

    case BUTTON_TYPE_PAUSE_MENU:
        break;

    default:
        exit(EXIT_FAILURE);
        break;
    }

    res->texture_rect = texture_rect;
    return res;
}

interface_button **create_all_buttons(SDL_Renderer *renderer) {

    interface_button **all_buttons = malloc(BUTTON_NUMBER * sizeof(SDL_Rect));
    assert(all_buttons != NULL);

    int chadburn_button_y =
        WINDOW_HEIGHT - CHADBURN_BODY_HEIGHT + CHADBURN_Y_BUTTON_PADDING;
    int right_chadburn_button_x =
        CHADBURN_BUTTON_WIDTH + 3 * CHADBURN_X_BUTTON_PADDING;

    // left chadburn button
    interface_button *button_chadburn_1 =
        create_button(CHADBURN_X_BUTTON_PADDING, chadburn_button_y,
                      BUTTON_TYPE_CHADBURN_DECREASE, renderer);
    // right chadburn button
    interface_button *button_chadburn_2 =
        create_button(right_chadburn_button_x, chadburn_button_y,
                      BUTTON_TYPE_CHADBURN_INCREASE, renderer);

    // centered button
    interface_button *button_cannon_1 = create_button(
        CARDS_BUTTON_START_X,
        BOTTOM_BAR_PADDING + (BOTTOM_BAR_HEIGHT - CARD_BUTTON_HEIGHT) / 2,
        BUTTON_TYPE_CANNON_1, renderer);

    /* interface_button *button_cannon_2 =
        create_button(CARDS_BUTTON_START_X +
                          1 * (CARD_BUTTON_WIDTH + X_DISTANCE_BETWEEN_CARDS),
                      BOTTOM_BAR_PADDING, BUTTON_TYPE_CANNON_2, renderer);

    interface_button *button_cannon_3 =
        create_button(CARDS_BUTTON_START_X +
                          2 * (CARD_BUTTON_WIDTH + X_DISTANCE_BETWEEN_CARDS),
                      BOTTOM_BAR_PADDING, BUTTON_TYPE_CANNON_1, renderer);

    interface_button *button_cannon_4 =
        create_button(CARDS_BUTTON_START_X +
                          3 * (CARD_BUTTON_WIDTH + X_DISTANCE_BETWEEN_CARDS),
                      BOTTOM_BAR_PADDING, BUTTON_TYPE_CANNON_1, renderer);
 */
    all_buttons[0] = button_chadburn_1;
    all_buttons[1] = button_chadburn_2;
    all_buttons[2] = button_cannon_1;
    // all_buttons[3] = button_cannon_2;
    // all_buttons[4] = button_cannon_3;
    // all_buttons[5] = button_cannon_4;

    return all_buttons;
}

void button_cooldown(interface_button **all_buttons) {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        interface_button *button = all_buttons[i];

        // if it's the last frame before being clickable
        if (button->click_cooldown == 1) {
            button->texture_rect->x = 0; // reset normal texture
        }
        if (button->click_cooldown > 0) {
            button->click_cooldown--;
        }
    }
}

void free_all_buttons(interface_button **all_buttons) {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        interface_button *button = all_buttons[i];
        free(button->texture_rect);

        // removing the line below doesn't seem to leak more bytes (?)
        // SDL_DestroyTexture(button->texture);

        free(button);
    }
    free(all_buttons);
}

void init_chadburn_graphic(chadburn_graphic *graphic, SDL_Renderer *renderer) {

    // on-screen chadburn body position & dimensions
    graphic->body_rect.x = 0;                                    // left
    graphic->body_rect.y = WINDOW_HEIGHT - CHADBURN_BODY_HEIGHT; // bottom
    graphic->body_rect.w = CHADBURN_BODY_WIDTH;
    graphic->body_rect.h = CHADBURN_BODY_HEIGHT;

    // selector
    graphic->selector_rect.x = 0;
    graphic->selector_rect.y = graphic->body_rect.y; // aligned with body
    graphic->selector_rect.w = CHADBURN_SELECTOR_WIDTH;
    graphic->selector_rect.h = CHADBURN_SELECTOR_HEIGHT;

    // load body image in texture
    image_to_texture("../images/chadburn_body.png", &graphic->body_texture,
                     NULL, NULL, renderer);

    int selector_texture_width, selector_texture_height;
    image_to_texture("../images/chadburn_selector_sheet.png",
                     &graphic->selector_texture, &selector_texture_width,
                     &selector_texture_height, renderer);

    graphic->selector_texture_rect = malloc(sizeof(SDL_Rect));

    // the texture has 2 rows and 3 columns
    graphic->selector_texture_rect->w = selector_texture_width / 3;
    graphic->selector_texture_rect->h = selector_texture_height / 2;

    // start in neutral position: second sprite in the sheet
    graphic->selector_texture_rect->x = 1 * (graphic->selector_texture_rect->w);
    graphic->selector_texture_rect->y = 0;

    return;
}

void free_chadburn_graphic(chadburn_graphic graphic) {
    assert(graphic.selector_texture_rect != NULL);
    free(graphic.selector_texture_rect);
    return;
}

void draw_chadburn_graphic(SDL_Renderer *renderer, chadburn_graphic *graphic) {
    SDL_RenderCopy(renderer, graphic->body_texture, NULL, &graphic->body_rect);
    SDL_RenderCopy(renderer, graphic->selector_texture,
                   graphic->selector_texture_rect, &graphic->selector_rect);
    return;
}

void set_speed_chadburn_graphic(chadburn_graphic *graphic,
                                chadburn_speed speed) {
    switch (speed) {

    case CHADBURN_SPEED_FULL_REVERSE:
        graphic->selector_texture_rect->x = 0;
        graphic->selector_texture_rect->y = 0;
        break;

    case CHADBURN_SPEED_HALF_REVERSE:
        graphic->selector_texture_rect->x = graphic->selector_texture_rect->w;
        graphic->selector_texture_rect->y = 0;
        break;

    case CHADBURN_SPEED_NEUTRAL:
        graphic->selector_texture_rect->x =
            graphic->selector_texture_rect->w * 2;
        graphic->selector_texture_rect->y = 0;
        break;

    case CHADBURN_SPEED_THIRD_AHEAD:
        graphic->selector_texture_rect->x = 0;
        graphic->selector_texture_rect->y = graphic->selector_texture_rect->h;
        break;

    case CHADBURN_SPEED_TWO_THIRDS_AHEAD:
        graphic->selector_texture_rect->x = graphic->selector_texture_rect->w;
        graphic->selector_texture_rect->y = graphic->selector_texture_rect->h;
        break;

    case CHADBURN_SPEED_FULL_AHEAD:
        graphic->selector_texture_rect->x =
            graphic->selector_texture_rect->w * 2;
        graphic->selector_texture_rect->y = graphic->selector_texture_rect->h;
        break;

    default:
        fprintf(stderr,
                "Error in set_speed_chadburn_graphic: wrong speed input (%d)\n",
                speed);
        exit(EXIT_FAILURE);
        break;
    }
    return;
}

void init_ui_bg_graphic(ui_bg_graphic *bg, SDL_Renderer *renderer) {
    bg->rect = (SDL_Rect){
        .x = 0,
        .y = WINDOW_HEIGHT - BOTTOM_BAR_HEIGHT,
        .w = WINDOW_WIDTH,
        .h = BOTTOM_BAR_HEIGHT,
    };
    image_to_texture("../images/bottom_bar_bg_ui.png", &bg->texture, NULL, NULL,
                     renderer);
    return;
}

void draw_ui_bg(SDL_Renderer *renderer, ui_bg_graphic *bg) {
    SDL_RenderCopy(renderer, bg->texture, NULL, &(bg->rect));
}

void free_ui_bg_graphic(ui_bg_graphic bg) {
    SDL_DestroyTexture(bg.texture);
}