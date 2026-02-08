#ifndef STATE_H
#define STATE_H

#include <stdio.h>

#define WIDTH 1500
#define HEIGHT 800
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_BLUE 0x1414b8

#define RAYS_NUMBER 100
#define RAY_THICKNESS 1
#define COLOR_RAY_1 0xffec99
#define COLOR_RAY_2 0xffe066
#define COLOR_RAY_3 0xffd43b
#define COLOR_RAY_4 0xe6bf35
#define COLOR_RAY_5 0xccaa2f
#define COLOR_RAY_6 0xb39529

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
    DECREASE_WIDTH,
    ROTATE,

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
    double angle;
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
    double x, y, w, h;
    double angle;
} mirror_t;

typedef struct
{
    circle_t *circles;
    size_t circle_count;

    light_source_t *light_sources;
    size_t light_source_count;

    rectangle_t *rectangles;
    size_t rectangle_count;

    mirror_t *mirrors;
    size_t mirror_count;

    selection_t selection;

} state_t;

#endif // !STATE_H
