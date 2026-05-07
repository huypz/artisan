#pragma once

#include "godot_cpp/classes/control.hpp"
#include "godot_cpp/classes/node.hpp"

class Main : public godot::Node {
    GDCLASS(Main, godot::Node)

private:
    godot::Node *world = nullptr;
    godot::Control *gui = nullptr;

protected:
    static void _bind_methods();

public:
    void _ready() override;
};
