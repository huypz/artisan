#pragma once

#include "godot_cpp/variant/vector3.hpp"

class VoxelColumn {

protected:
    static void _bind_methods();

public:
    godot::Vector3 world_position;
};
