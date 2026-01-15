#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "draw.h"
#include "state.h"
#include "update.h"

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RAY_1 0xffec99
#define COLOR_RAY_2 0xffe066
#define COLOR_RAY_3 0xffd43b
#define COLOR_RAY_4 0xe6bf35
#define COLOR_RAY_5 0xccaa2f
#define COLOR_RAY_6 0xb39529
#define RAYS_NUMBER 150
#define RAY_THICKNESS 1

void *get_selected_object(state_t *s)
{
    switch (s->selection.type)
    {
    case SELECT_CIRCLE:
        return &s->circles[s->selection.index];
    case SELECT_LIGHT:
        return &s->light_sources[s->selection.index];
    case SELECT_RECTANGLE:
        return &s->rectangles[s->selection.index];
    case SELECT_MIRROR:
        return &s->mirrors[s->selection.index];
    default:
        return NULL;
    }
}

void change_obj_by_pattern(pattern_t pattern, int w, int h, int r)
{
}

void track_input(pattern_t pattern, state_t *state)
{
    void *obj = get_selected_object(state);

    switch (state->selection.type)
    {
    }
}

void track_mouse_movement(double x, double y, state_t *state)
{
    void *obj = get_selected_object(state);

    if (state->selection.type == SELECT_CIRCLE)
    {
        circle_t *c = obj; // impliziter cast von void* zu circle_t*
        c->x = x;
        c->y = y;
    }
}

void move_circle(circle_t *c)
{

    if (c->y - c->r < 0)
    {
        c->v = -c->v;
    }

    if (c->y + c->r > HEIGHT)
    {
        c->v = -c->v;
    }
    c->y += c->v;
}

void move_rectangle(rectangle_t *t)
{
    if (t->x < 0)
    {
        t->v = -t->v;
    }

    if (t->x + t->w > WIDTH)
    {
        t->v = -t->v;
    }

    t->x += t->v;
}

void setup_state(state_t *s)
{
    // Object Cirlces
    s->circles = calloc(1, sizeof(circle_t));
    s->circles[0] = (circle_t){550, 300, 120, 1};
    s->circle_count = 1;

    // Light sources
    s->light_sources = calloc(1, sizeof(light_source_t));
    s->light_source_count = 1;
    // Setting circle
    s->light_sources[0].circle = (circle_t){200, 200, 40, 0};
    // rays allocating
    s->light_sources[0].rays = calloc(RAYS_NUMBER, sizeof(ray_t));

    // Object Rectangles
    s->rectangles = calloc(1, sizeof(rectangle_t));
    s->rectangle_count = 1;
    s->rectangles[0] = (rectangle_t){300, 40, 2, 100, 10};

    // Mirrors
    s->mirrors = calloc(1, sizeof(mirror_t));
    s->mirrors[0] = (mirror_t){300, 400, 2, 100, 80};
    s->mirror_count = 1;
}

void update_fps(int *fps, Uint32 *last_time, double *frame_counter)
{
    Uint32 current = SDL_GetTicks();
    Uint32 delta = current - *last_time;

    if (delta > 0)
    {
        *fps = 1000 / delta;
    }

    *last_time = current;
    (*frame_counter)++;

    if (*frame_counter >= 120)
    {
        printf("FPS: %d\n", *fps);
        *frame_counter = 0;
    }
}

int main(void)
{

    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

    if (!window)
    {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    srand((unsigned)time(NULL));

    state_t state;

    setup_state(&state);

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    double mouse_x = 0;
    double mouse_y = 0;

    // FPS
    double frame_counter = 0;
    Uint32 last_time = 0;
    int fps = 0;

    SDL_Event e;
    int simulation_running = 1;
    int auto_movement = 1;

    generate_rays(state.light_sources, state.light_source_count);

    while (simulation_running)
    {

        SDL_FillRect(surface, NULL, COLOR_BLACK); // NULL -> fill full surface
        draw_circles(surface, state.circles, state.circle_count, COLOR_WHITE);
        draw_rectangles(surface, state.rectangles, state.rectangle_count, COLOR_WHITE);
        draw_rectangles(surface, state.mirrors, state.mirror_count, COLOR_WHITE);
        draw_all_rays(surface, &state);
        draw_light_sources(surface, state.light_sources, state.light_source_count, COLOR_WHITE);
        SDL_UpdateWindowSurface(window);

        if (auto_movement)
        {
            move_circle(&state.circles[0]);
            move_rectangle(&state.rectangles[0]);
        }

        update_fps(&fps, &last_time, &frame_counter);
        SDL_Delay(10); // 100 fps

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
            { // if x icon pressed, without couldnt quit,
              // except terminating in ps
                simulation_running = 0;
            }

            if (e.type == SDL_MOUSEMOTION && e.motion.state != 0)
            {
                mouse_x = e.motion.x;
                mouse_y = e.motion.y;
                state.light_sources[0].circle.x = mouse_x;
                state.light_sources[0].circle.y = mouse_y;
                generate_rays(state.light_sources, state.light_source_count);
            }

            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_RETURN)
                {
                    auto_movement = !auto_movement;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
