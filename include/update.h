#ifndef UPDATE_H
#define UPDATE_H

#include "state.h"

int hit_circles(int x, int y, circle_t circles[], int circle_count);
int in_mirror(int x, int y, mirror_t *mirrors, int mirrors_count);
int hit_rectangles(int x, int y, rectangle_t *rectangles, int rectangle_count);
void check_hit_mirrors(ray_t *ray, mirror_t *mirrors, int mirrors_count);
int out_of_screen(int x, int y);
void generate_rays(light_source_t *light_sources, int light_source_count);

#endif
