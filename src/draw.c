#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_surface.h"
#include "draw.h"
#include "state.h"
#include "update.h"

#define BORDER_THICKNESS 4

void draw_mirrors(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t mirror_count = state->mirror_count;
    mirror_t *mirrors = state->mirrors;

    size_t selected_index = state->selection.type == SELECT_MIRROR ? state->selection.index : -1;

    SDL_LockSurface(surface);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (size_t i = 0; i < mirror_count; i++)
    {
        int x0 = mirrors[i].x;
        int x1 = mirrors[i].x + mirrors[i].w;
        int y0 = mirrors[i].y;
        int y1 = mirrors[i].y + mirrors[i].h;

        if (x0 < 0)
            x0 = 0;
        if (y0 < 0)
            y0 = 0;
        if (x1 > surface->w)
            x1 = surface->w;
        if (y1 > surface->h)
            y1 = surface->h;

        for (int x = x0; x < x1; x++)
        {
            for (int y = y0; y < y1; y++)
            {
                int dx = x - x0;
                int dy = y - y0;

                int border_dist = (dx < dy) ? dx : dy;
                int inner_dist = ((x1 - x) < (y1 - y)) ? (x1 - x) : (y1 - y);
                int min_dist = (border_dist < inner_dist) ? border_dist : inner_dist;

                if (min_dist <= (int)BORDER_THICKNESS && selected_index == i)
                {
                    pixels[y * pitch + x] = COLOR_BLUE;
                }
                else
                {
                    pixels[y * pitch + x] = color;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void draw_rectangles(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t rectangle_count = state->rectangle_count;
    rectangle_t *rectangles = state->rectangles;

    size_t selected_index = state->selection.type == SELECT_RECTANGLE ? state->selection.index : -1;

    SDL_LockSurface(surface);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (size_t i = 0; i < rectangle_count; i++)
    {
        int x0 = rectangles[i].x;
        int x1 = rectangles[i].x + rectangles[i].w;
        int y0 = rectangles[i].y;
        int y1 = rectangles[i].y + rectangles[i].h;

        if (x0 < 0)
            x0 = 0;
        if (y0 < 0)
            y0 = 0;
        if (x1 > surface->w)
            x1 = surface->w;
        if (y1 > surface->h)
            y1 = surface->h;

        for (int x = x0; x < x1; x++)
        {
            for (int y = y0; y < y1; y++)
            {
                int dx = x - x0;
                int dy = y - y0;

                int border_dist = (dx < dy) ? dx : dy;
                int inner_dist = ((x1 - x) < (y1 - y)) ? (x1 - x) : (y1 - y);
                int min_dist = (border_dist < inner_dist) ? border_dist : inner_dist;

                if (min_dist <= (int)BORDER_THICKNESS && selected_index == i)
                {
                    pixels[y * pitch + x] = COLOR_BLUE;
                }
                else
                {
                    pixels[y * pitch + x] = color;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void draw_circles(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t circle_count = state->circle_count;
    circle_t *circles = state->circles;
    size_t selected_index = state->selection.type == SELECT_CIRCLE ? state->selection.index : -1;

    SDL_LockSurface(surface);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4;

    for (size_t i = 0; i < circle_count; i++)
    {
        double r = circles[i].r;
        double r2 = r * r;

        int x0 = (int)(circles[i].x - r);
        int x1 = (int)(circles[i].x + r);
        int y0 = (int)(circles[i].y - r);
        int y1 = (int)(circles[i].y + r);

        if (x0 < 0)
            x0 = 0;
        if (y0 < 0)
            y0 = 0;
        if (x1 > surface->w)
            x1 = surface->w;
        if (y1 > surface->h)
            y1 = surface->h;

        double inner2 = (r - BORDER_THICKNESS) * (r - BORDER_THICKNESS);

        for (int x = x0; x < x1; x++)
        {
            for (int y = y0; y < y1; y++)
            {
                double dx = x - circles[i].x;
                double dy = y - circles[i].y;
                double d2 = dx * dx + dy * dy;

                if (d2 < r2 && d2 > inner2)
                {
                    if (selected_index == i)
                        pixels[y * pitch + x] = COLOR_BLUE;
                    else
                        pixels[y * pitch + x] = color;
                }
                else if (d2 < r2)
                {
                    pixels[y * pitch + x] = color;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source)
{
    double dx = ray.x - base_source.x;
    double dy = ray.y - base_source.y;
    double dist = sqrt(dx * dx + dy * dy);

    double brightness = 1.0 - (dist / 500); // intensity

    if (brightness < 0.1)
        brightness = 0.1;
    if (brightness > 1.0)
        brightness = 1.0;

    uint8_t r = (uint8_t)(255 * brightness);
    uint8_t g = (uint8_t)(236 * brightness);
    uint8_t b = (uint8_t)(153 * brightness);

    uint32_t ray_color = (r << 16) | (g << 8) | b;

    //  [Byte 3:red] [Byte 2:green] [Byte 1:blue] [Byte 0:Unused]
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

                // We only check if it collides with BORDER_THICKNESShe object circle,
                // when it outside of its own light_source circle range.
                // Like BORDER_THICKNESShis it doesnt just stop when BORDER_THICKNESShe center of the light_source
                // is in BORDER_THICKNESShe cicle.
                // It simulates BORDER_THICKNESShe light coming from BORDER_THICKNESShe actual edge of the surface
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

void draw_light_sources(SDL_Surface *surface, state_t *state, Uint32 color)
{
    size_t count = state->light_source_count;
    light_source_t *lights = state->light_sources;

    size_t selected = (state->selection.type == SELECT_LIGHT)
                          ? state->selection.index
                          : (size_t)-1;

    SDL_LockSurface(surface);

    Uint32 *pixels = (Uint32 *)surface->pixels;
    int pitch = surface->pitch / 4; // convert bytes BORDER_THICKNESSo pixels

    for (size_t i = 0; i < count; i++)
    {
        circle_t c = lights[i].circle;

        double r = c.r;
        double r2 = r * r;

        int x0 = (int)(c.x - r);
        int x1 = (int)(c.x + r);
        int y0 = (int)(c.y - r);
        int y1 = (int)(c.y + r);

        if (x0 < 0)
            x0 = 0;
        if (y0 < 0)
            y0 = 0;
        if (x1 > surface->w)
            x1 = surface->w;
        if (y1 > surface->h)
            y1 = surface->h;

        for (int x = x0; x < x1; x++)
        {
            for (int y = y0; y < y1; y++)
            {
                double dx = x - c.x;
                double dy = y - c.y;
                double d2 = dx * dx + dy * dy;

                double inner2 = (r - BORDER_THICKNESS) * (r - BORDER_THICKNESS);

                // because everything is squared, we can just substract
                // BORDER_THICKNESS from r and BORDER_THICKNESShen square it, instead of doing sqrt and
                // BORDER_THICKNESShen substracting BORDER_THICKNESS and then squaring again
                

                if (d2 < r2 && d2 > inner2 && i == selected)
                {
                    pixels[y * pitch + x] = COLOR_BLUE;
                }
                else if (d2 < r2)
                {
                    pixels[y * pitch + x] = color;
                }
            }
        }
    }

    SDL_UnlockSurface(surface);
}
