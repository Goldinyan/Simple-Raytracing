#include <math.h>
#include <stdio.h>

#include "state.h"
#include "update.h"

#define HIT_BUFFER_DISTANCE 3

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
    double prev_x = ray->x - ray->dx;
    double prev_y = ray->y - ray->dy;

    int bfr = HIT_BUFFER_DISTANCE;

    {
        for (int i = 0; i < mirrors_count; i++)
        {

            mirror_t m = mirrors[i];
            double x = ray->x;
            double y = ray->y;

            double d_x1 = fabs(ray->x - m.x);
            double d_x2 = fabs(ray->x - (m.x + m.w));

            double d_y1 = fabs(ray->y - m.y);
            double d_y2 = fabs(ray->y - (m.y + m.h));

            if (d_x1 > bfr && d_x2 > bfr && d_y1 > bfr && d_y2 > bfr)
                continue;

            if ((d_y1 < bfr || d_y2 < bfr) &&
                x >= m.x &&
                x <= m.x + m.w)
            {
                ray->dy = -ray->dy;
            }

            if ((d_x1 < bfr || d_x2 < bfr) &&
                y >= m.y &&
                y <= m.y + m.h)
            {
                ray->dx = -ray->dx;
            }
        }
    }
    for (int i = 0; i < mirrors_count; i++)
    {
        double x = ray->x;
        double y = ray->y;

        double d_x1 = fabs(ray->x - mirrors[i].x);
        double d_x2 = fabs(ray->x - (mirrors[i].x + mirrors[i].w));

        double d_y1 = fabs(ray->y - mirrors[i].y);
        double d_y2 = fabs(ray->y - (mirrors[i].y + mirrors[i].h));

        if (d_x1 > 1 && d_x2 > 1 && d_y1 > 1 && d_y2 > 1)
            continue;

        if ((d_y1 < 1 || d_y2 < 1) &&
            x >= mirrors[i].x &&
            x <= mirrors[i].x + mirrors[i].w)
        {
            ray->dy = -ray->dy;
        }

        if ((d_x1 < 1 || d_x2 < 1) &&
            y >= mirrors[i].y &&
            y <= mirrors[i].y + mirrors[i].h)
        {
            ray->dx = -ray->dx;
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
