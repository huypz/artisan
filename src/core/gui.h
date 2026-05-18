#pragma once

#include "godot_cpp/classes/control.hpp"

class GUI : public godot::Control {
    GDCLASS(GUI, godot::Control)

protected:
    static void _bind_methods();

public:
    void _ready() override;

    void show_editor_ui();

private:
    godot::Control* editor_ui = nullptr;
};
