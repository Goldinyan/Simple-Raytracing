#include <math.h>
#include <stdio.h>

#include "state.h"
#include "update.h"

int hit_circles(int x, int y, circle_t circles[], int circle_count)
{
    for (int i = 0; i < circle_count; i++)
    {
        double dx = x - circles[i].x;
        double dy = y - circles[i].y;
        if (dx * dx + dy * dy <= circles[i].r * circles[i].r)
            return 1;
    }

    return 0;
}

int in_mirror(int x, int y, mirror_t *mirrors, int mirrors_count)
{
    for (int i = 0; i < mirrors_count; i++)
    {
        if (x >= mirrors[i].x &&
            x <= mirrors[i].x + mirrors[i].w &&
            y >= mirrors[i].y &&
            y <= mirrors[i].y + mirrors[i].h)
            return 1;
    }

    return 0;
}

int hit_rectangles(int x, int y, rectangle_t *rectangles, int rectangle_count)
{
    for (int i = 0; i < rectangle_count; i++)
    {
        if (x >= rectangles[i].x &&
            x <= rectangles[i].x + rectangles[i].w &&
            y >= rectangles[i].y &&
            y <= rectangles[i].y + rectangles[i].h)
            return 1;
    }

    return 0;
}

void check_hit_mirrors(ray_t *ray, mirror_t *mirrors, int mirrors_count)
{
    for (int i = 0; i < mirrors_count; i++)
    {
        if ((fabs(ray->x - mirrors[i].x) < 0.7 || fabs(ray->x - (mirrors[i].x + mirrors[i].w)) < 0.7) &&
            ray->y >= mirrors[i].y &&
            ray->y <= mirrors[i].y + mirrors[i].h)
        {
            ray->dx = -ray->dx;
        }

        if ((fabs(ray->y - mirrors[i].y) < 0.7 || fabs(ray->y - (mirrors[i].y + mirrors[i].h)) < 0.7) &&
            ray->x >= mirrors[i].x &&
            ray->x <= mirrors[i].x + mirrors[i].w)
        {
            ray->dy = -ray->dy;
        }
    }
}

int out_of_screen(int x, int y)
{
    return x < 0 || x > WIDTH || y < 0 || y > HEIGHT;
}

void generate_rays(light_source_t *light_sources, int light_source_count)
{
    for (int x = 0; x < light_source_count; x++)
    {
        for (size_t i = 0; i < RAYS_NUMBER; i++)
        {
            double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
            double dy = sin(angle);
            double dx = cos(angle);
            ray_t ray = {light_sources[x].circle.x, light_sources[x].circle.y, dx, dy};
            light_sources[x].rays[i] = ray;
        }
    }
}
