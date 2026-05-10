#pragma once

#include <godot_cpp/variant/vector3.hpp>

class VoxelMetrics {

public:
    static constexpr float LENGTH_X = 1.0f,
                           LENGTH_Y = 1.0f,
                           LENGTH_Z = 1.0f;

    static const int CHUNK_SIZE_X = 8,
                     CHUNK_SIZE_Y = 1,
                     CHUNK_SIZE_Z = 8;

    static const godot::Vector3 CORNERS[];

};
