#include "entity.h"

using namespace godot;

void Entity::_bind_methods() {}

void Entity::_ready() {
    pivot = memnew(Node3D);
    pivot->set_name("Pivot");
    add_child(pivot);
}
