#include <stdio.h>

#include "draw.h"
#include "state.h"
#include "update.h"


void draw_mirrors(SDL_Surface *surface, mirror_t *mirrors, int rectangle_count, Uint32 color)
{
    for (int i = 0; i < rectangle_count; i++)
    {
        SDL_Rect rect = (SDL_Rect){mirrors[i].x, mirrors[i].y, mirrors[i].w, mirrors[i].h};
        SDL_FillRect(surface, &rect, color);
    }
}

void draw_rectangles(SDL_Surface *surface, rectangle_t *rectangles, int rectangle_count, Uint32 color)
{
    for (int i = 0; i < rectangle_count; i++)
    {
        SDL_Rect rect = (SDL_Rect){rectangles[i].x, rectangles[i].y, rectangles[i].w, rectangles[i].h};
        SDL_FillRect(surface, &rect, color);
    }
}

void draw_circles(SDL_Surface *surface, circle_t circles[], int circle_count, Uint32 color)
{
    for (int i = 0; i < circle_count; i++)
    {

        double radius_squared = pow(circles[i].r, 2);

        // Only looping through the rectangle created from the cirle for perfomance

        for (double x = circles[i].x - circles[i].r; x < circles[i].x + circles[i].r; x++)
        {
            for (double y = circles[i].y - circles[i].r; y < circles[i].y + circles[i].r; y++)
            {

                double distance_squared = pow(x - circles[i].x, 2) + pow(y - circles[i].y, 2);

                if (distance_squared < radius_squared)
                {
                    SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                    SDL_FillRect(surface, &pixel, color);
                }
            }
        }
    }
}

void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source)
{
    int r = rand() % 300;
    double dist;

    double dx = ray.x - base_source.x;
    double dy = ray.y - base_source.y;

    dist = (dx * dx + dy * dy) / (200 + r);
    // printf("%f\n", dist);

    uint32_t ray_color;

    if (dist < 50)
        ray_color = COLOR_RAY_1;
    else if (dist < 100)
        ray_color = COLOR_RAY_2;
    else if (dist < 150)
        ray_color = COLOR_RAY_3;
    else if (dist < 200)
        ray_color = COLOR_RAY_4;
    else if (dist < 250)
        ray_color = COLOR_RAY_5;
    else
        ray_color = COLOR_RAY_6;

    SDL_Rect ray_point =
        (SDL_Rect){ray.x, ray.y, RAY_THICKNESS, RAY_THICKNESS};
    SDL_FillRect(surface, &ray_point, ray_color);
}

void draw_all_rays(SDL_Surface *surface, state_t *state)
{
    for (size_t i = 0; i < state->light_source_count; i++)
    {

        double light_source_radius_squared = pow(state->light_sources[i].circle.r, 2);

        for (size_t x = 0; x < RAYS_NUMBER; x++)
        {

            ray_t ray = state->light_sources[i].rays[x];

            while (1)
            {

                ray.x += ray.dx;
                ray.y += ray.dy;

                draw_single_ray(surface, ray, state->light_sources[i].circle);

                // We only check if it collides with the object circle,
                // when it outside of its own light_source circle range.
                // Like this it doesnt just stop when the center of the light_source
                // is in the cicle.
                // It simulates the light coming from the actual edge of the surface
                // we garantuee

                if (out_of_screen(ray.x, ray.y))
                    break;

                double dx = ray.x - state->light_sources[i].circle.x;
                double dy = ray.y - state->light_sources[i].circle.y;

                double light_source_distance_squared = dx * dx + dy * dy;

                if (light_source_distance_squared > light_source_radius_squared)
                {

                    if (hit_circles(ray.x, ray.y, state->circles, state->circle_count))
                        break;

                    if (hit_rectangles(ray.x, ray.y, state->rectangles, state->rectangle_count))
                        break;

                    if (in_mirror(ray.x, ray.y, state->mirrors, state->mirror_count))
                        break;
                    check_hit_mirrors(&ray, state->mirrors, state->mirror_count);

                    // mirror collision horizontal
                }
            }
        }
    }
}

void draw_light_sources(SDL_Surface *surface,
                        light_source_t *lights,
                        size_t count,
                        Uint32 color)
{
    for (size_t i = 0; i < count; i++)
    {
        circle_t *c = &lights[i].circle;
        draw_circles(surface, c, 1, color);
    }
}
