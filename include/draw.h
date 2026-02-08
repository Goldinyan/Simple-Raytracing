#ifndef DRAW_H
#define DRAW_H

#include "state.h"
#include <SDL2/SDL.h>

void draw_mirrors(SDL_Surface *surface, state_t *state, Uint32 color);
void draw_rectangles(SDL_Surface *surface, state_t *state, Uint32 color);
void draw_circles(SDL_Surface *surface, state_t *state, Uint32 color);
void draw_light_sources(SDL_Surface *surface, state_t *state, Uint32 color);

void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source);
void draw_all_rays(SDL_Surface *surface, state_t *state);

#endif // !DRAW_H
