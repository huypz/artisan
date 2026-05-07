#include "main.h"

#include "gui.h"
#include "world.h"

void Main::_bind_methods() {}

void Main::_ready() {
    world = memnew(World);
    world->set_name("World");
    add_child(world);

    gui = memnew(GUI);
    gui->set_name("GUI");
    add_child(gui);
}
