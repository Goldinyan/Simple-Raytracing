#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keycode.h"
#include <SDL2/SDL.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RAY 0xffd43b
#define RAYS_NUMBER 150
#define RAY_THICKNESS 1

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

    // Misc

} state_t;

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

void draw_rectangle(SDL_Surface *surface, rectangle_t rectangle, Uint32 color)
{
    for (double x = rectangle.x; x < rectangle.x + rectangle.w; x++)
    {

        for (double y = rectangle.y; y < rectangle.y + rectangle.h; y++)
        {
            SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
            SDL_FillRect(surface, &pixel, color);
        }
    }
}

void draw_circle(SDL_Surface *surface, circle_t circle, Uint32 color)
{

    double radius_squared = pow(circle.r, 2);

    // Only looping through the rectangle created from the cirle for perfomance

    for (double x = circle.x - circle.r; x < circle.x + circle.r; x++)
    {
        for (double y = circle.y - circle.r; y < circle.y + circle.r; y++)
        {

            double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);

            if (distance_squared < radius_squared)
            {
                SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

int hit_cirlce(int x, int y, circle_t circle)
{
    double dx = x - circle.x;
    double dy = y - circle.y;
    return dx * dx + dy * dy <= circle.r * circle.r;
}

int hit_rectangle(int x, int y, rectangle_t rect)
{
    return x >= rect.x &&
           x <= rect.x + rect.w &&
           y >= rect.y &&
           y <= rect.y + rect.h;
}

void check_hit_mirror(ray_t *ray, rectangle_t mirror)
{
    if ((fabs(ray->x - mirror.x) < 0.5 || fabs(ray->x - (mirror.x + mirror.w)) < 0.5) &&
        ray->y >= mirror.y &&
        ray->y <= mirror.y + mirror.h)
    {
        ray->dx = -ray->dx;
    }

    if ((fabs(ray->y - mirror.y) < 0.5 || fabs(ray->y - (mirror.y + mirror.h)) < 0.5) &&
        ray->x >= mirror.x &&
        ray->x <= mirror.x + mirror.w)
    {
        ray->dy = -ray->dy;
    }
}

int out_of_screen(int x, int y)
{
    return x < 0 || x > WIDTH || y < 0 || y > HEIGHT;
}

void draw_all_rays(SDL_Surface *surface, ray_t rays[RAYS_NUMBER], Uint32 color,
                   circle_t object_circle, rectangle_t object_rectangle, circle_t light_source, rectangle_t mirror)
{

    double light_sourece_radius_squared = pow(light_source.r, 2);

    for (size_t i = 0; i < RAYS_NUMBER; i++)
    {

        ray_t ray = rays[i];

        size_t reached_end_of_screen = 0;

        while (!reached_end_of_screen)
        {

            ray.x += ray.dx;
            ray.y += ray.dy;

            // RAY POINT

            SDL_Rect ray_point =
                (SDL_Rect){ray.x, ray.y, RAY_THICKNESS, RAY_THICKNESS};
            SDL_FillRect(surface, &ray_point, color);

            // POINT END

            // We only check if it collides with the object circle,
            // when it outside of its own light_source circle range.
            // Like this it doesnt just stop when the center of the light_source
            // is in the cicle.
            // It simulates the light coming from the actual edge of the surface
            // we garantuee

            if (out_of_screen(ray.x, ray.y))
                break;

            double dx = ray.x - light_source.x;
            double dy = ray.y - light_source.y;

            double light_source_distance_squared = dx * dx + dy * dy;

            if (light_source_distance_squared > light_sourece_radius_squared)
            {

                if (hit_cirlce(ray.x, ray.y, object_circle))
                    break;

                if (hit_rectangle(ray.x, ray.y, object_rectangle))
                    break;

                check_hit_mirror(&ray, mirror);

                // mirror collision horizontal
            }
        }
    }
}

void generate_rays(light_source_t light_sources[], int light_source_count)
{ // array is empty

    for (int x = 0; x < light_source_count; x++)
    {
        light_source_t light_source = light_sources[x];

        for (size_t i = 0; i < RAYS_NUMBER; i++)
        {
            double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
            double dy = sin(angle);
            double dx = cos(angle);
            ray_t ray = {light_source.circle.x, light_source.circle.y, dx, dy};
            light_source.rays[i] = ray;
        }
    }
}

void setup_state(state_t *s)
{
    s->rays = calloc(RAYS_NUMBER, sizeof(ray_t));
    s->ray_count = RAYS_NUMBER;

    s->circles = calloc(1, sizeof(circle_t));
    s->circles[0] = (circle_t){550, 300, 120, 1};
    s->circle_count = 1;

    s->light_sources = calloc(1, sizeof(light_source_circle));
    s->light_source_count = 1;
    // Setting circle
    s->light_sources[0].circle = (circle_t){200, 200, 40, 0};
    // rays allocating
    s->light_sources[0].rays = calloc(RAYS_NUMBER, sizeof(ray_t));

    s->rectangles = calloc(1, sizeof(rectangle_t));
    s->rectangle_count = 1;
    s->rectangles[0] = (rectangle_t){300, 40, 2, 100, 10};

    s->mirrors = calloc(1, sizeof(rectangle_t));
    s->mirrors[0] = (rectangle_t){300, 400, 2, 100, 80};
    s->mirror_count = 1;
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

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    SDL_Event e;
    int simulation_running = 1;
    int auto_movement = 1;

    generate_rays(state.light_sources, rays);

    while (simulation_running)
    {

        SDL_FillRect(surface, NULL, COLOR_BLACK); // NULL -> fill full surface
        draw_circle(surface, shadow_circle, COLOR_WHITE);
        draw_rectangle(surface, shadow_rectangle, COLOR_WHITE);
        draw_all_rays(surface, rays, COLOR_RAY, shadow_circle, shadow_rectangle, circle, mirror);
        draw_circle(surface, circle, COLOR_WHITE);
        draw_rectangle(surface, mirror, COLOR_WHITE);
        SDL_UpdateWindowSurface(window);

        if (auto_movement)
        {
            move_circle(&shadow_circle);
            move_rectangle(&shadow_rectangle);
        }

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
                circle.x = e.motion.x;
                circle.y = e.motion.y;
                generate_rays(circle, rays);
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
