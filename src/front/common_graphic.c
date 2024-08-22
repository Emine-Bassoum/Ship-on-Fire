#include "common_graphic.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

void image_to_texture(const char *image_path, SDL_Texture **texture_dest,
                      int *texture_width, int *texture_height,
                      SDL_Renderer *renderer) {

    SDL_Surface *surface; // temporary, used for texture creation
    surface = IMG_Load(image_path);
    if (!surface) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error in sprite surface init: %s", IMG_GetError());
        exit(EXIT_FAILURE);
    }

    *texture_dest = SDL_CreateTextureFromSurface(renderer, surface);
    if (!(*texture_dest)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
                     "Error in sprite texture init: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_FreeSurface(surface);

    if (SDL_QueryTexture(*texture_dest, NULL, NULL, texture_width,
                         texture_height)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Error in query texture: %s",
                     SDL_GetError());
        exit(EXIT_FAILURE);
    }

    return;
}
