#include "geometry.h"
#include <SDL2/SDL_rect.h>

double deg_to_rad(double deg) {
    return deg * PI / 180.0;
}

double rad_to_deg(double rad) {
    return rad * 180.0 / PI;
}

SDL_Rect shift_rect(SDL_Rect rect, SDL_Point vect) {
    return (SDL_Rect){rect.x + vect.x, rect.y + vect.y, rect.w, rect.h};
}

SDL_Point shift_point(SDL_Point point, SDL_Point vect) {
    return (SDL_Point){point.x + vect.x, point.y + vect.y};
}

SDL_Point rect_coords(SDL_Rect rect) {
    return (SDL_Point){rect.x, rect.y};
}