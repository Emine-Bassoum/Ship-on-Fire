/* Internal geometry defined here for points, shapes, intersections... */

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <SDL2/SDL_rect.h>

#define PI 3.14159265358979323846264338327950288

// Angle conversion: degree to radian
double deg_to_rad(double deg);

// Angle conversion: radian to degree
double rad_to_deg(double rad);

/* Return a copy of the rectangle rect shifted along the vector vect. */
SDL_Rect shift_rect(SDL_Rect rect, SDL_Point vect);

/* Return a copy of the point shifted along the vector vect. */
SDL_Point shift_point(SDL_Point point, SDL_Point vect);

/* Return the (x, y) values of rect. */
SDL_Point rect_coords(SDL_Rect rect) ;

#endif