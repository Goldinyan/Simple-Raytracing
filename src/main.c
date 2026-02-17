#include "SDL2/SDL_events.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdlib.h>

#include "draw.h"
#include "state.h"
#include "update.h"

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

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

void track_input(pattern_t pattern, state_t *state)
{
    void *obj = get_selected_object(state);

    switch (state->selection.type)
    {
    case SELECT_CIRCLE:
    {
        circle_t *c = obj;
        if (pattern == INCREASE_HEIGHT || pattern == INCREASE_WIDTH)
            c->r++;
        else if (pattern == DECREASE_HEIGHT || pattern == DECREASE_WIDTH)
            c->r--;
    }
    break;

    case SELECT_RECTANGLE:
    {
        rectangle_t *r = obj;
        switch (pattern)
        {
        case INCREASE_WIDTH:
            r->w++;
            break;
        case INCREASE_HEIGHT:
            r->h++;
            break;
        case DECREASE_WIDTH:
            r->w--;
            break;

        case DECREASE_HEIGHT:
            r->h--;
            break;
        case ROTATE:
            r->angle++;
            break;
        }
    }
    break;

    case SELECT_LIGHT:
    {
        light_source_t *l = obj;

        if (pattern == INCREASE_HEIGHT || pattern == INCREASE_WIDTH)
            l->circle.r++;
        else if (pattern == DECREASE_HEIGHT || pattern == DECREASE_WIDTH)
            l->circle.r--;
    }
    break;

    case SELECT_MIRROR:
    {
        mirror_t *m = obj;
        switch (pattern)
        {
        case INCREASE_WIDTH:
            m->w++;
            break;
        case INCREASE_HEIGHT:
            m->h++;
            break;
        case DECREASE_WIDTH:
            m->w--;
            break;

        case DECREASE_HEIGHT:
            m->h--;
            break;
        case ROTATE:
            m->angle++;
            break;
        }
    }
    break;

    case SELECT_NONE:
        break;

    default:
        break;
    }
}

void track_mouse_movement(double x, double y, state_t *state)
{
    void *obj = get_selected_object(state);

    switch (state->selection.type)
    {
    case SELECT_CIRCLE:
    {
        circle_t *c = obj; // impliziter cast von void* zu circle_t*
        c->x = x;
        c->y = y;
    }
    break;

    case SELECT_RECTANGLE:
    {
        rectangle_t *r = obj;
        r->x = x;
        r->y = y;
    }
    break;

    case SELECT_LIGHT:
    {
        light_source_t *l = obj;
        l->circle.x = x;
        l->circle.y = y;
    }
    break;

    case SELECT_MIRROR:
    {
        mirror_t *m = obj;
        m->x = x;
        m->y = y;
    }
    break;

    case SELECT_NONE:
        break;

    default:
        break;
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
    s->light_sources = calloc(2, sizeof(light_source_t));
    s->light_source_count = 1;
    s->light_sources[0].circle = (circle_t){200, 200, 40, 0};
    s->light_sources[0].rays = calloc(RAYS_NUMBER, sizeof(ray_t));

    // Object Rectangles
    s->rectangles = calloc(1, sizeof(rectangle_t));
    s->rectangle_count = 1;
    s->rectangles[0] = (rectangle_t){300, 40, 2, 100, 10, 0};

    // Mirrors
    s->mirrors = calloc(1, sizeof(mirror_t));
    s->mirrors[0] = (mirror_t){300, 400, 50, 100, 0};
    s->mirror_count = 1;

    s->selection.type = SELECT_LIGHT;
    s->selection.index = 0;
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

    if (*frame_counter >= 240)
    {
        printf("FPS: %d\n", *fps);
        *frame_counter = 0;
    }
}

void add_to_array(void **array, size_t *count, void *element, size_t element_size)
{
    // **array weil ich den pointer ändern muss
    void *new_array = realloc(*array, (*count + 1) * element_size);
    // realloc nimmt den alten pointer und die neue größe,
    // gibt den neuen pointer zurück, oder NULL wenn fehler
    if (new_array)
    {
        *array = new_array;
        memcpy((char *)(*array) + (*count * element_size), element, element_size);

        // die funktion memcpy kopiert element_size bytes
        // von element zu *array + (*count * element_size)
        // und das funktioniert weil element_size
        // die größe eines elements ist, und *count die aktuelle anzahl
        // der elemente im array, also *count * element_size gibt
        // die byte position des nächsten freien slots im array an.
        // und char * ist notwendig, weil pointer arithmetik in c immer
        // die größe des datentyps berücksichtigt,
        // also wenn ich *array + (*count * element_size) schreiben würde,
        // würde es um (*count * element_size) * sizeof(void*) bytes verschieben,
        // was nicht das ist was ich will.
        // Mit char* wird es um genau (*count * element_size) bytes verschoben,
        // da sizeof(char) immer 1 ist.

        (*count)++;
        // count erhöhen
    }
    else
    {
        // Handle error
        fprintf(stderr, "Failed to allocate memory for array expansion.\n");
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
        draw_circles(surface, &state, COLOR_WHITE);
        draw_rectangles(surface, &state, COLOR_WHITE);
        draw_mirrors(surface, &state, COLOR_WHITE);
        draw_all_rays(surface, &state);
        draw_light_sources(surface, &state, COLOR_WHITE);
        SDL_UpdateWindowSurface(window);

        if (auto_movement)
        {
            move_circle(&state.circles[0]);
            move_rectangle(&state.rectangles[0]);
        }

        update_fps(&fps, &last_time, &frame_counter);
        SDL_Delay(5); // 200 fps

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
                track_mouse_movement(mouse_x, mouse_y, &state);
                generate_rays(state.light_sources, state.light_source_count);
            }

            if (e.type == SDL_KEYDOWN)
            {
                SDL_Keycode key = e.key.keysym.sym;

                switch (key)
                {

                // SCALING
                case SDLK_UP:
                    track_input(INCREASE_HEIGHT, &state);
                    break;
                case SDLK_DOWN:
                    track_input(DECREASE_HEIGHT, &state);
                    break;
                case SDLK_LEFT:
                    track_input(DECREASE_WIDTH, &state);
                    break;
                case SDLK_RIGHT:
                    track_input(INCREASE_WIDTH, &state);
                    break;

                    // SPAWNING

                case SDLK_c:
                    add_to_array((void **)&state.circles, &state.circle_count,
                                 &(circle_t){mouse_x, mouse_y, 40, 0}, sizeof(circle_t));
                    break;

                case SDLK_r:
                    add_to_array((void **)&state.rectangles, &state.rectangle_count,
                                 &(rectangle_t){mouse_x, mouse_y, 0, 100, 50, 0}, sizeof(rectangle_t));
                    break;

                case SDLK_l:
                    add_to_array((void **)&state.light_sources, &state.light_source_count,
                                 &(light_source_t){.circle = {mouse_x, mouse_y, 40, 0}, .rays = calloc(RAYS_NUMBER, sizeof(ray_t))}, sizeof(light_source_t));
                    generate_rays(state.light_sources, state.light_source_count);
                    break;

                case SDLK_m:
                    add_to_array((void **)&state.mirrors, &state.mirror_count,
                                 &(mirror_t){mouse_x, mouse_y, 50, 100, 0}, sizeof(mirror_t));
                    break;

                case SDLK_RETURN:
                    auto_movement = !auto_movement;
                    break;

                default:
                    break;
                }
            }
        }
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
