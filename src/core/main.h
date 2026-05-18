#pragma once

#include "godot_cpp/classes/node.hpp"

class World;
class GUI;

class Main : public godot::Node {
    GDCLASS(Main, godot::Node)

private:
    World *world = nullptr;
    GUI *gui = nullptr;

protected:
    static void _bind_methods();

public:
    void _ready() override;
};
