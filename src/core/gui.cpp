#include "gui.h"

#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/packed_scene.hpp>

using namespace godot;

void GUI::_bind_methods() {}

void GUI::_ready() {
    set_anchors_preset(Control::PRESET_FULL_RECT);

    auto load_ui = [&](const String& path) -> Control* {
        Ref<PackedScene> scene = ResourceLoader::get_singleton()->load(path);
        ERR_FAIL_COND_V(scene.is_null(), nullptr);

        Control* ui = Object::cast_to<Control>(scene->instantiate());
        ui->set_visible(false);
        add_child(ui);
        return ui;
    };

    editor_ui = load_ui("res://scenes/ui/editor_ui.tscn");
    show_editor_ui();
}

void GUI::show_editor_ui() {
    editor_ui->set_visible(true);
}
