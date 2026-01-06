#include <SDL2/SDL.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>

#define WIDTH 1200
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_RAY 0xffd43b
#define RAYS_NUMBER 150
#define RAY_THICKNESS 1

typedef struct {
  double x;
  double y;
  double r;
  double v; // speed
} circle_t;

typedef struct {
  double x_start, y_start;
  double angle;
} ray_t;

void move_circle(circle_t *c) {

  if (c->y - c->r < 0) {
    c->v = -c->v;
  }

  if (c->y + c->r > HEIGHT) {
    c->v = -c->v;
  }
  c->y += c->v;
}

void draw_circle(SDL_Surface *surface, circle_t circle, Uint32 color) {

  double radius_squared = pow(circle.r, 2);

  // Only looping through the rectangle created from the cirle for perfomance

  for (double x = circle.x - circle.r; x < circle.x + circle.r; x++) {
    for (double y = circle.y - circle.r; y < circle.y + circle.r; y++) {

      double distance_squared = pow(x - circle.x, 2) + pow(y - circle.y, 2);

      if (distance_squared < radius_squared) {
        SDL_Rect pixel = (SDL_Rect){x, y, 1, 1};
        SDL_FillRect(surface, &pixel, color);
      }
    }
  }
}

void draw_rays(SDL_Surface *surface, ray_t rays[RAYS_NUMBER], Uint32 color,
               circle_t object, circle_t light_source) {

  double object_radius_squared = pow(object.r, 2);
  double light_sourece_radius_squared = pow(light_source.r, 2);

  for (size_t i = 0; i < RAYS_NUMBER; i++) {

    ray_t ray = rays[i];

    size_t reached_end_of_screen = 0;
    size_t obejct_hit = 0;

    double step = 1;
    double x_draw = ray.x_start;
    double y_draw = ray.y_start;
    while (!reached_end_of_screen && !obejct_hit) {

      x_draw += step * cos(ray.angle);
      y_draw += step * sin(ray.angle);

      // RAY POINT

      SDL_Rect ray_point =
          (SDL_Rect){x_draw, y_draw, RAY_THICKNESS, RAY_THICKNESS};
      SDL_FillRect(surface, &ray_point, color);

      // POINT END

      if (x_draw < 0 || x_draw > WIDTH)
        reached_end_of_screen = 1;

      if (y_draw < 0 || y_draw > HEIGHT)
        reached_end_of_screen = 1;

      // We only check if it collides with the object circle,
      // when it outside of its own light_source circle range.
      // Like this it doesnt just stop when the center of the light_source
      // is in the cicle.
      // It simulates the light coming from the actual edge of the surface
      // we garantuee


      double light_source_distance_squared =
          pow(x_draw - light_source.x, 2) + pow(y_draw - light_source.y, 2);

      double object_distance_squared =
          pow(x_draw - object.x, 2) + pow(y_draw - object.y, 2);

      if (light_source_distance_squared > light_sourece_radius_squared) {

        if (object_distance_squared < object_radius_squared) {
          break; // object hit
        }
      }
    }
  }
}

void generate_rays(circle_t c, ray_t rays[RAYS_NUMBER]) { // array is empty

  for (size_t i = 0; i < RAYS_NUMBER; i++) {
    double angle = ((double)i / RAYS_NUMBER) * 2 * M_PI;
    ray_t ray = {c.x, c.y, angle};
    rays[i] = ray;
  }
}

int main(void) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("SDL_Init error: %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window =
      SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);

  if (!window) {
    printf("SDL_CreateWindow error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  ray_t rays[RAYS_NUMBER];

  circle_t circle = {200, 200, 40, 0};
  circle_t shadow_circle = {550, 300, 120, 1};
  SDL_Surface *surface = SDL_GetWindowSurface(window);

  SDL_Event e;
  int simulation_running = 1;

  generate_rays(circle, rays);

  while (simulation_running) {

    SDL_FillRect(surface, NULL, COLOR_BLACK); // NULL -> fill full surface
    draw_circle(surface, shadow_circle, COLOR_WHITE);
    draw_rays(surface, rays, COLOR_RAY, shadow_circle, circle);
    draw_circle(surface, circle, COLOR_WHITE);
    SDL_UpdateWindowSurface(window);

    move_circle(&shadow_circle);

    SDL_Delay(10); // 100 fps

    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) { // if x icon pressed, without couldnt quit,
                                // except terminating in ps
        simulation_running = 0;
      }

      if (e.type == SDL_MOUSEMOTION && e.motion.state != 0) {
        circle.x = e.motion.x;
        circle.y = e.motion.y;
        generate_rays(circle, rays);
      }
    }
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
