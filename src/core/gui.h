#pragma once

#include "godot_cpp/classes/control.hpp"

class GUI : public godot::Control {
    GDCLASS(GUI, godot::Control)

protected:
    static void _bind_methods();
};
