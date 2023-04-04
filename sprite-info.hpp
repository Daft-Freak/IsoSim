#pragma once

#include <cstdint>

struct SpriteInfo {
    uint8_t sheet_x, sheet_y, sheet_w, sheet_h; // in 8x8 tiles
    uint8_t center_x, center_y; // in pixels
    uint8_t transform;
};

enum WallSide {
    Side_Bottom = 0,
    Side_Right,
    Side_Top,
    Side_Left
};

// isometric sprites from "iso-tile"
extern const SpriteInfo sprites[];

extern const int wall_id_start, wall_id_end;
extern const int object_id_start, object_id_end;

extern const uint8_t wall_to_hidden[];