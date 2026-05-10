#pragma once

#include <godot_cpp/classes/node3d.hpp>

class VoxelColumn : public godot::Node3D {
    GDCLASS(VoxelColumn, godot::Node3D)

protected:
    static void _bind_methods();

};
