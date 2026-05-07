#pragma once

#include "godot_cpp/classes/node3d.hpp"

class Grid : public godot::Node3D {
    GDCLASS(Grid, godot::Node3D)

protected:
    static void _bind_methods();
};
