#pragma once

#include <godot_cpp/classes/camera3d.hpp>

class Camera : public godot::Camera3D {
    GDCLASS(Camera, godot::Camera3D)

private:
    godot::Node3D *target = nullptr;

protected:
    static void _bind_methods();

public:
    void _ready() override;

    void set_target(godot::Node3D *new_target);
};
