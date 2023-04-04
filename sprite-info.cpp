#include "sprite-info.hpp"

#include "graphics/surface.hpp"

using blit::SpriteTransform;

const SpriteInfo sprites[] {
    { 0,  3, 4, 3, 16, 16, 0}, // floor
    {12, 20, 4, 2, 16,  8, 0}, // grass
    {}, // don't need to rotate the floor
    {},

    // wall
    { 4,  0, 2, 6, 16, 40, 0}, // bottom
    { 4,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL}, // right
    { 4,  0, 2, 6,  0, 48, 0}, // top
    { 4,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL}, // left

    // wall + window
    {12,  0, 2, 6, 16, 40, 0},
    {12,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL},
    {12,  0, 2, 6,  0, 48, 0},
    {12,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL},

    // wall + door
    {20,  0, 2, 6, 16, 40, 0},
    {20,  0, 2, 6,  0, 40, SpriteTransform::HORIZONTAL},
    {20,  0, 2, 6,  0, 48, 0},
    {20,  0, 2, 6, 16, 48, SpriteTransform::HORIZONTAL},

    // hidden wall
    {28,  0, 2, 2, 16,  8, 0},
    {28,  0, 2, 2,  0,  8, SpriteTransform::HORIZONTAL},
    {28,  0, 2, 2,  0, 16, 0},
    {28,  0, 2, 2, 16, 16, SpriteTransform::HORIZONTAL},
    // hidden wall + door
    {28,  4, 2, 2, 16,  8, 0},
    {28,  4, 2, 2,  0,  8, SpriteTransform::HORIZONTAL},
    {28,  4, 2, 2,  0, 16, 0},
    {28,  4, 2, 2, 16, 16, SpriteTransform::HORIZONTAL},

    // furniture may need some adjusting

    // bed　(24)
    { 0,  6, 6, 4, 16, 25, 0},
    { 6,  6, 6, 4, 32, 25, 0},
    {12,  6, 6, 4, 16, 24, 0},
    {18,  6, 6, 4, 32, 24, 0},

    // tv (28)
    {24,  6, 3, 3, 24, 24, 0},
    {27,  8, 3, 3,  0, 24, 0},
    { 0, 10, 3, 3, 18, 24, 0},
    { 3, 10, 3, 3,  2, 25, 0},

    // kitchen sink (32)
    { 6, 10, 4, 4, 14, 25, 0},
    {10, 10, 4, 4, 15, 25, 0},
    {14, 10, 4, 4, 16, 24, 0},
    {18, 10, 4, 4, 15, 24, 0},

    // oven (36)
    {22, 10, 4, 4, 14, 25, 0},
    {26, 11, 4, 4, 15, 25, 0},
    { 0, 13, 4, 4, 16, 24, 0},
    { 4, 14, 4, 4, 15, 24, 0},

    // fridge (40)
    { 0, 17, 3, 5,  8, 38, 0},
    { 3, 18, 3, 5, 16, 38, 0},
    { 6, 18, 3, 5, 16, 32, 0},
    { 9, 14, 3, 5,  8, 32, 0},

    // shower (44)
    {12, 14, 4, 6, 16, 40, 0},
    {16, 14, 4, 6, 16, 40, 0},
    {20, 14, 4, 6, 16, 40, 0},
    {24, 15, 4, 6, 16, 40, 0},

    // bathroom sink (48)
    {28, 15, 2, 3,  2, 24, 0},
    {30, 14, 2, 3, 14, 24, 0},
    {28, 18, 2, 3, 14, 16, 0},
    {30, 17, 2, 3,  2, 16, 0},

    // toilet (52)
    { 0, 22, 3, 3,  8, 20, 0},
    { 3, 23, 3, 3, 16, 20, 0},
    { 6, 23, 3, 2, 16, 12, 0},
    { 9, 19, 3, 2,  8, 12, 0},

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