#pragma once

#include <cstdint>

struct Voxel {
    uint8_t type = 0;
    uint8_t face_tex[6] = {};
};
