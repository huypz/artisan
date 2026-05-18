#include "world.h"

#include "voxel/voxel_grid.h"

void World::_bind_methods() {}

void World::_ready() {
    grid = memnew(VoxelGrid);
    grid->set_name("VoxelGrid");
    add_child(grid);
}
