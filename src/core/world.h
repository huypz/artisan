#pragma once

#include "godot_cpp/classes/node3d.hpp"

class World : public godot::Node3D {
    GDCLASS(World, godot::Node3D)

protected:
    static void _bind_methods();
};
