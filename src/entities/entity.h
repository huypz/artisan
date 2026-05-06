#ifndef ENTITY_H
#define ENTITY_H

#include "godot_cpp/classes/node3d.hpp"
#include "godot_cpp/classes/character_body3d.hpp"

class Entity : public godot::CharacterBody3D {
    GDCLASS(Entity, godot::CharacterBody3D)

protected:
    godot::Node3D *pivot = nullptr;

    static void _bind_methods();

public:
    virtual godot::Node3D *get_pivot() const { return pivot; }

    void _ready() override;
};

#endif
