#ifndef DRAW_H
#define DRAW_H

#include "state.h"
#include <SDL2/SDL.h>

void draw_mirrors(SDL_Surface *surface, mirror_t *mirrors, int rectangle_count, Uint32 color);
void draw_rectangles(SDL_Surface *surface, rectangle_t *rectangles, int rectangle_count, Uint32 color);
void draw_circles(SDL_Surface *surface, circle_t circles[], int circle_count, Uint32 color);
void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source);
void draw_all_rays(SDL_Surface *surface, state_t *state);
void draw_light_sources(SDL_Surface *surface, light_source_t *lights,
                        size_t count,
                        Uint32 color);

#endif // !DRAW_H
