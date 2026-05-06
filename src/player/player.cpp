#include "player.h"


#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_map.hpp>
#include <godot_cpp/classes/input_event_key.hpp>\

using namespace godot;

void Player::_bind_methods() {}

void Player::_ready() {
    Entity::_ready();
    setup_input();

    character = memnew(Sprite3D);
    character->set_name("Character");
    character->set_texture(ResourceLoader::get_singleton()->load("res://assets/textures/characters/wizard.png"));
    character->set_pixel_size(0.1);
    pivot->add_child(character);
}

void Player::_process(double delta) {

}

void Player::_physics_process(double delta) {
    Vector3 velocity = get_velocity();

    // input direction
    Vector2 input = Vector2(
        Input::get_singleton()->get_axis("move_left", "move_right"),
        Input::get_singleton()->get_axis("move_back", "move_forward")
    );

    if (input.length_squared() > 0.0f) {
        // camera-relative direction
        float angle = camera_angle; // your global uniform
        Vector3 right   = Vector3( cos(angle), 0.0f, -sin(angle));
        Vector3 forward = Vector3(-sin(angle), 0.0f, -cos(angle));

        Vector3 direction = (right * input.x + forward * input.y).normalized();
        velocity.x = direction.x * speed;
        velocity.z = direction.z * speed;
    } else {
        velocity.x = 0.0f;
        velocity.z = 0.0f;
    }

    set_velocity(velocity);
    move_and_slide();
}

void Player::setup_input() {
    InputMap *input_map = InputMap::get_singleton();

    auto add_action = [&](const String &action, Key key) {
        input_map->add_action(action);
        Ref<InputEventKey> event;
        event.instantiate();
        event->set_keycode(key);
        input_map->action_add_event(action, event);
    };

    add_action("move_right", Key::KEY_D);
    add_action("move_left", Key::KEY_A);
    add_action("move_back", Key::KEY_S);
    add_action("move_forward", Key::KEY_W);
}
