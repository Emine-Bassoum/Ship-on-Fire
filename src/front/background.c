#include "background.h"
#include "common_graphic.h"
#include "user_interface.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdio.h>

void init_sea_graphic(SDL_Renderer *renderer, sea_graphic *sea, int x, int y,
                      int w, int h) {
    sea->x = x;
    sea->rect = (SDL_Rect){x, y - BOTTOM_BAR_HEIGHT, w, h};
    image_to_texture("../images/sea.png", &sea->texture, NULL, NULL, renderer);
}

void draw_sea_graphic(SDL_Renderer *renderer, sea_graphic *sea_graphic) {
    SDL_RenderCopy(renderer, sea_graphic->texture, NULL, &(sea_graphic->rect));
}

void init_sun(SDL_Renderer *renderer, sun *sun) {
    sun->rect.y = 100;
    sun->rect.x = WINDOW_WIDTH - 150;
    // load texture and set rect dimensions to texture dimensions
    image_to_texture("../images/sun.png", &sun->texture, &sun->rect.w,
                     &sun->rect.h, renderer);
}

void draw_sun(SDL_Renderer *renderer, sun *sun) {
    // Render the sun texture
    SDL_RenderCopy(renderer, sun->texture, NULL, &(sun->rect));
}

void init_sky(SDL_Renderer *renderer, sky *sky) {
    sky->rect.x = 0;
    sky->rect.y = 0;
    sky->rect.h = WINDOW_HEIGHT - GRAPHICAL_SEA_HEIGHT - BOTTOM_BAR_HEIGHT;
    // load texture
    image_to_texture("../images/sky.png", &sky->texture, &sky->rect.w, NULL,
                     renderer);
}

void draw_sky(SDL_Renderer *renderer, sky *sky) {
    // Render the sky texture
    SDL_RenderCopy(renderer, sky->texture, NULL, &(sky->rect));
}

void init_cloud(SDL_Renderer *renderer, cloud *cloud, int cloud_number,
                int speed) {
    cloud->speed = speed;

    if (cloud_number == 1) {
        cloud->rect.y = 150; // distance between top of window and top of cloud
        image_to_texture("../images/cloud1.png", &cloud->texture,
                         &cloud->rect.w, NULL, renderer);

    } else if (cloud_number == 2) {
        cloud->rect.y = 75;
        image_to_texture("../images/cloud2.png", &cloud->texture,
                         &cloud->rect.w, NULL, renderer);
    } else {
        fprintf(stdout, "Error in init_cloud: wrong cloud number.");
        exit(EXIT_FAILURE);
    }
    cloud->rect.h = WINDOW_HEIGHT - GRAPHICAL_SEA_HEIGHT - BOTTOM_BAR_HEIGHT -
                    cloud->rect.y;
    return;
}

void move_and_draw_cloud(SDL_Renderer *renderer, cloud *cloud) {
    cloud->rect.x -= cloud->speed;
    // If the cloud has moved off the screen
    if (cloud->rect.x + cloud->rect.w < 0) {
        // Reset its position to the right edge of the screen
        cloud->rect.x = cloud->rect.w / 2;
    }
    SDL_RenderCopy(renderer, cloud->texture, NULL, &(cloud->rect));
    // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0x00);
    // SDL_RenderFillRect(renderer, &cloud->rect);
    return;
}

void init_background(SDL_Renderer *renderer, background_graphic *bg) {
    init_sea_graphic(renderer, &bg->sea_1, 0,
                     WINDOW_HEIGHT - GRAPHICAL_SEA_HEIGHT, WINDOW_WIDTH,
                     GRAPHICAL_SEA_HEIGHT);
    init_sea_graphic(renderer, &bg->sea_2, WINDOW_WIDTH,
                     WINDOW_HEIGHT - GRAPHICAL_SEA_HEIGHT, WINDOW_WIDTH,
                     GRAPHICAL_SEA_HEIGHT);
    init_sun(renderer, &bg->sun);
    init_sky(renderer, &bg->sky);
    init_cloud(renderer, &bg->cloud1_1, 1, 2);
    bg->cloud1_1.rect.x = -bg->cloud1_1.rect.w / 2;
    init_cloud(renderer, &bg->cloud1_2, 1, 2);
    bg->cloud1_2.rect.x = bg->cloud1_2.rect.w / 2;
    init_cloud(renderer, &bg->cloud2_1, 2, 1);
    bg->cloud2_1.rect.x = -bg->cloud2_1.rect.w / 2;
    init_cloud(renderer, &bg->cloud2_2, 2, 1);
    bg->cloud2_2.rect.x = bg->cloud2_2.rect.w / 2;
}

void draw_background(SDL_Renderer *renderer, background_graphic *bg) {
    draw_sky(renderer, &bg->sky);
    draw_sun(renderer, &bg->sun);
    move_and_draw_cloud(renderer, &bg->cloud2_1);
    move_and_draw_cloud(renderer, &bg->cloud2_2);
    move_and_draw_cloud(renderer, &bg->cloud1_1);
    move_and_draw_cloud(renderer, &bg->cloud1_2);
    draw_sea_graphic(renderer, &bg->sea_1);
    draw_sea_graphic(renderer, &bg->sea_2);
}

void free_background(background_graphic *bg) {
    SDL_DestroyTexture(bg->sea_1.texture);
    SDL_DestroyTexture(bg->sea_2.texture);
    SDL_DestroyTexture(bg->sun.texture);
    SDL_DestroyTexture(bg->sky.texture);
    SDL_DestroyTexture(bg->cloud1_1.texture);
    SDL_DestroyTexture(bg->cloud1_2.texture);
    SDL_DestroyTexture(bg->cloud2_1.texture);
    SDL_DestroyTexture(bg->cloud2_2.texture);
    return;
}