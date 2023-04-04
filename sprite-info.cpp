#include "sprite-info.hpp"

#include "graphics/surface.hpp"

using blit::SpriteTransform;

const SpriteInfo sprites[] {
    { 0,  3, 4, 3, 16, 16, 0}, // floor
    { 4,  0, 4, 2, 16,  8, 0}, // grass
    {}, // don't need to rotate the floor
    {},

    // wall
    { 8,  0, 2, 6, 16, 40, 0}, // bottom
    { 8,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL}, // right
    { 8,  0, 2, 6,  0, 48, 0}, // top
    { 8,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL}, // left

    // wall + window
    {10,  0, 2, 6, 16, 40, 0},
    {10,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL},
    {10,  0, 2, 6,  0, 48, 0},
    {10,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL},

    // wall + door
    {12,  0, 2, 6, 16, 40, 0},
    {12,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL},
    {12,  0, 2, 6,  0, 48, 0},
    {12,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL},

    // hidden wall
    {14,  0, 2, 2, 16,  8, 0},
    {14,  0, 2, 2,  0,  8, SpriteTransform::HORIZONTAL},
    {14,  0, 2, 2,  0, 16, 0},
    {14,  0, 2, 2, 16, 16, SpriteTransform::HORIZONTAL},
    // hidden wall + door
    {14,  2, 2, 2, 16,  8, 0},
    {14,  2, 2, 2,  0,  8, SpriteTransform::HORIZONTAL},
    {14,  2, 2, 2,  0, 16, 0},
    {14,  2, 2, 2, 16, 16, SpriteTransform::HORIZONTAL},

    // furniture may need some adjusting

    // bed　(24)
    { 0,  6, 6, 4, 16, 25, 0},
    { 6,  6, 6, 4, 32, 25, 0},
    {12,  6, 6, 4, 16, 24, 0},
    {18,  6, 6, 4, 32, 24, 0},

    // tv (28)
    {24,  6, 3, 3, 24, 24, 0},
    {27,  6, 3, 3,  0, 24, 0},
    { 0, 10, 3, 3, 18, 24, 0},
    { 3, 10, 3, 3,  2, 25, 0},

    // kitchen sink (32)
    { 6, 10, 4, 4, 14, 25, 0},
    {10, 10, 4, 4, 15, 25, 0},
    {14, 10, 4, 4, 16, 24, 0},
    {18, 10, 4, 4, 15, 24, 0},

    // oven (36)
    {22, 10, 4, 4, 14, 25, 0},
    {26,  9, 4, 4, 15, 25, 0},
    { 0, 13, 4, 4, 16, 24, 0},
    { 4, 14, 4, 4, 15, 24, 0},

    // fridge (40)
    { 8, 14, 3, 5,  8, 38, 0},
    {11, 14, 3, 5, 16, 38, 0},
    {14, 14, 3, 5, 16, 32, 0},
    {17, 14, 3, 5,  8, 32, 0},

    // shower (44)
    {16,  0, 4, 6, 16, 40, 0},
    {20,  0, 4, 6, 16, 40, 0},
    {24,  0, 4, 6, 16, 40, 0},
    {28,  0, 4, 6, 16, 40, 0},

    // bathroom sink (48)
    {30,  6, 2, 3,  2, 24, 0},
    {30,  9, 2, 3, 14, 24, 0},
    {28, 13, 2, 3, 14, 16, 0},
    {30, 12, 2, 3,  2, 16, 0},

    // toilet (52)
    {20, 14, 3, 3,  8, 20, 0},
    {23, 14, 3, 3, 16, 20, 0},
    {20, 17, 3, 2, 16, 12, 0},
    {23, 17, 3, 2,  8, 12, 0},

    // placeholder thing
    { 0, 0, 2, 2, 8, 16, 0},
};

const int wall_id_start = 4;
const int wall_id_end = 16;
const int object_id_start = 24;
const int object_id_end = 56;

const uint8_t wall_to_hidden[]{
    0, // floor
    4, // wall
    4, // wall + window
    5, // wall + door
};