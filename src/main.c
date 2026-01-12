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

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RAY_1 0xffe066
#define COLOR_RAY_2 0xffec99
#define COLOR_RAY_3 0xffd43b
#define COLOR_RAY_4 0xe6bf35
#define COLOR_RAY_5 0xccaa2f
#define COLOR_RAY_6 0xb39529
#define RAYS_NUMBER 150
#define RAY_THICKNESS 1

typedef enum
{
    SELECT_NONE,
    SELECT_CIRCLE,
    SELECT_LIGHT,
    SELECT_RECTANGLE,
    SELECT_MIRROR
} selection_type_t;

typedef enum
{
    INCREASE_HEIGHT,
    INCREASE_WIDTH,
    DECREASE_HEIGHT,
    DECREASE_WIDTH

} pattern_t;

typedef struct
{
    selection_type_t type;
    size_t index; // Index im jeweiligen Array
} selection_t;

typedef struct
{
    double x;
    double y;
    double r;
    double v; // speed
} circle_t;

typedef struct
{
    double x, y, v;
    double w, h;
} rectangle_t;

typedef struct
{
    double x, y;
    double dx, dy;
} ray_t;

typedef struct
{
    circle_t circle;
    ray_t *rays;

} light_source_t;

typedef struct
{
    circle_t *circles;
    size_t circle_count;

    light_source_t *light_sources;
    size_t light_source_count;

    rectangle_t *rectangles;
    size_t rectangle_count;

    rectangle_t *mirrors;
    size_t mirror_count;

    selection_t selection;

} state_t;

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

void draw_rectangles(SDL_Surface *surface, rectangle_t rectangles[], int rectangle_count, Uint32 color)
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

int hit_rectangles(int x, int y, rectangle_t rectangles[], int rectangle_count)
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

void check_hit_mirrors(ray_t *ray, rectangle_t mirrors[], int mirrors_count)
{
    for (int i = 0; i < mirrors_count; i++)
    {
        if ((fabs(ray->x - mirrors[i].x) < 0.5 || fabs(ray->x - (mirrors[i].x + mirrors[i].w)) < 0.5) &&
            ray->y >= mirrors[i].y &&
            ray->y <= mirrors[i].y + mirrors[i].h)
        {
            ray->dx = -ray->dx;
        }

        if ((fabs(ray->y - mirrors[i].y) < 0.5 || fabs(ray->y - (mirrors[i].y + mirrors[i].h)) < 0.5) &&
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

void draw_single_ray(SDL_Surface *surface, ray_t ray, circle_t base_source)
{

    double dist;

    double dx = ray.x - base_source.x;
    double dy = ray.y - base_source.y;

    dist = dx * dx + dy * dy;

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
        ray_color = COLOR_RAY_3;

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

            size_t reached_end_of_screen = 0;

            while (!reached_end_of_screen)
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

                    check_hit_mirrors(&ray, state->mirrors, state->mirror_count);

                    // mirror collision horizontal
                }
            }
        }
    }
}

void generate_rays(light_source_t *light_sources[], int light_source_count)
{ // array is empty

    for (int x = 0; x < light_source_count; x++)
    {

        for (size_t i = 0; i < RAYS_NUMBER; i++)
        {
            double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
            double dy = sin(angle);
            double dx = cos(angle);
            ray_t ray = {light_sources[x]->circle.x, light_sources[x]->circle.y, dx, dy};
            light_sources[x]->rays[i] = ray;
        }
    }
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
    s->mirrors = calloc(1, sizeof(rectangle_t));
    s->mirrors[0] = (rectangle_t){300, 400, 2, 100, 80};
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

    generate_rays(&state.light_sources, state.light_source_count);

    while (simulation_running)
    {

        SDL_FillRect(surface, NULL, COLOR_BLACK); // NULL -> fill full surface
        draw_circles(surface, state.circles, state.circle_count, COLOR_WHITE);
        draw_rectangles(surface, state.rectangles, state.rectangle_count, COLOR_WHITE);
        draw_rectangles(surface, state.mirrors, state.mirror_count, COLOR_WHITE);
        draw_all_rays(surface, &state);
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
                generate_rays(&state.light_sources, state.light_source_count);
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
