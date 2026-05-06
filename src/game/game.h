#ifndef GAME_H
#define GAME_H

#include <godot_cpp/classes/node.hpp>

class Game : public godot::Node {
    GDCLASS(Game, godot::Node)

protected:
    static void _bind_methods();

public:
    void _ready() override;
};

#endif
