#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_surface.h"
#include "draw.h"
#include "state.h"
#include "update.h"

void draw_mirrors(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t mirror_count = state->mirror_count;
    mirror_t *mirrors = state->mirrors;

    size_t selected_index = state->selection.type == SELECT_MIRROR ? state->selection.index : -1;

    for (size_t i = 0; i < mirror_count; i++)
    {
        if (selected_index == i)
        {
            SDL_Rect rect_bg = (SDL_Rect){mirrors[i].x - 2, mirrors[i].y - 2, mirrors[i].w + 4, mirrors[i].h + 4};
            SDL_FillRect(surface, &rect_bg, COLOR_BLUE);
        }
        SDL_Rect rect = (SDL_Rect){mirrors[i].x, mirrors[i].y, mirrors[i].w, mirrors[i].h};
        SDL_FillRect(surface, &rect, color);
    }
}

void draw_rectangles(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t rectangle_count = state->rectangle_count;
    rectangle_t *rectangles = state->rectangles;

    size_t selected_index = state->selection.type == SELECT_RECTANGLE ? state->selection.index : -1;

    for (size_t i = 0; i < rectangle_count; i++)
    {
        if (selected_index == i)
        {
            SDL_Rect rect_bg = (SDL_Rect){rectangles[i].x - 2, rectangles[i].y - 2, rectangles[i].w + 4, rectangles[i].h + 4};
            SDL_FillRect(surface, &rect_bg, COLOR_BLUE);
        }
        SDL_Rect rect = (SDL_Rect){rectangles[i].x, rectangles[i].y, rectangles[i].w, rectangles[i].h};
        SDL_FillRect(surface, &rect, color);
    }
}

void draw_circles(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t circle_count = state->circle_count;
    circle_t *circles = state->circles;
    size_t selected_index = state->selection.type == SELECT_CIRCLE ? state->selection.index : -1;

    for (size_t i = 0; i < circle_count; i++)
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
                    if (selected_index == i)
                    {
                        SDL_Rect pixel_bg = (SDL_Rect){x - 1, y - 1, 3, 3};
                        SDL_FillRect(surface, &pixel_bg, COLOR_BLUE);
                    }
                    SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                    SDL_FillRect(surface, &pixel, color);
                }
            }
        }
    }
}

void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source)
{
    double dx = ray.x - base_source.x;
    double dy = ray.y - base_source.y;
    double dist = sqrt(dx * dx + dy * dy);

    double brightness = 1.0 - (dist / 500.0);
    if (brightness < 0.1)
        brightness = 0.1;
    if (brightness > 1.0)
        brightness = 1.0;

    uint8_t r = (uint8_t)(255 * brightness);
    uint8_t g = (uint8_t)(236 * brightness);
    uint8_t b = (uint8_t)(153 * brightness);

    uint32_t ray_color = (r << 16) | (g << 8) | b;

//  [Byte 3:Red] [Byte 2:Green] [Byte 1:Blue] [Byte 0:Unused] 
//  16 - 23      8 - 15           0 - 7

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
                        state_t *state, Uint32 color)
{
    size_t light_source_count = state->light_source_count;
    light_source_t *light_source = state->light_sources;
    size_t selected_index = state->selection.type == SELECT_LIGHT ? state->selection.index : -1;

    for (size_t i = 0; i < light_source_count; i++)
    {

        for (size_t i = 0; i < light_source_count; i++)
        {

            circle_t circle = light_source[i].circle;

            double radius_squared = pow(circle.r, 2);

            // Only loopng through the rectangle created from the cirle for perfomance

            for (double x = circle.x - circle.r; x < circle.x + circle.r; x++)
            {
                for (double y = circle.y - circle.r; y < circle.y + circle.r; y++)
                {

                    double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);

                    if (distance_squared < radius_squared)
                    {
                        if (selected_index == i)
                        {
                            SDL_Rect pixel_bg = (SDL_Rect){x - 1, y - 1, 3, 3};
                            SDL_FillRect(surface, &pixel_bg, COLOR_BLUE);
                        }

                        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                        SDL_FillRect(surface, &pixel, color);
                    }
                }
            }
        }
    }
}
