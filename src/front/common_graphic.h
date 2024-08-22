/* Useful general-purpose graphical functions. */

#ifndef COMMON_GRAPHIC_H
#define COMMON_GRAPHIC_H

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>

/**
 * Load an image into a texture, while :
 *  - performing all security checks
 *  - setting dimensions of the texture
 *
 * @param image_path path to image that will be loaded
 * @param texture_dest pointer to pointer filled in with texture
 * @param texture_width pointer filled in with the texture width, can be NULL if
 * information not needed
 * @param texture_height pointer filled in with the texture height, can be NULL
 * if information not needed
 * @param renderer pointer to renderer that'll be associated with texture
 */
void image_to_texture(const char *image_path, SDL_Texture **texture_dest,
                      int *texture_width, int *texture_height,
                      SDL_Renderer *renderer);

#endif /* COMMON_GRAPHIC_H */