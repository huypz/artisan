#pragma once

#include <godot_cpp/templates/local_vector.hpp>

#include "voxel.h"

class VoxelColumn {

protected:
    static void _bind_methods();

public:
    float world_x;
    float world_y;
    float world_z;

    VoxelColumn();

    Voxel& operator[](int y);

    int voxel_count() const;


private:
    godot::LocalVector<Voxel> voxels;
};
