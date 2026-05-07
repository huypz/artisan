#pragma once

#include <godot_cpp/classes/sprite3d.hpp>

#include "entities/entity.h"

class Player : public Entity {
    GDCLASS(Player, Entity)

private:
    godot::Sprite3D *character = nullptr;

    float speed = 10.0f;
    float camera_angle = 0.0f;

    void setup_input();

protected:
    static void _bind_methods();

public:
    void _ready() override;
    void _process(double delta) override;
    void _physics_process(double delta) override;
};
