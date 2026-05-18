#include "main.h"

#include "gui.h"
#include "world.h"
#include "voxel/voxel_grid.h"

using namespace godot;

void Main::_bind_methods() {}

void Main::_ready() {
    world = memnew(World);
    world->set_name("World");
    add_child(world);

    gui = memnew(GUI);
    gui->set_name("GUI");
    add_child(gui);

    Node* editor_ui = gui->get_node<Node>("EditorUI");
    VoxelGrid* grid = world->grid;
    ERR_FAIL_NULL(editor_ui);
    ERR_FAIL_NULL(grid);
    editor_ui->connect("brush_tile_id_changed", callable_mp(grid, &VoxelGrid::set_brush_tile_id));
}
