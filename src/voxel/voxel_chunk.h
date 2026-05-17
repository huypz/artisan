#pragma once

#include <godot_cpp/classes/node3d.hpp>

#include "voxel/voxel_mesh.h"

class VoxelColumn;

class VoxelChunk : public godot::Node3D {
    GDCLASS(VoxelChunk, godot::Node3D)

protected:
    static void _bind_methods();

public:
    void _enter_tree() override;
    void _exit_tree() override;

    void assign_column(int index, VoxelColumn* column);

    void build_buffer();
    void upload_buffer();
    void free_mesh();

    VoxelColumn* get_column(int local_x, int local_z);

private:
    godot::LocalVector<VoxelColumn*> columns;

    VoxelMesh mesh;
    godot::PackedFloat32Array buffer;
    godot::RID multimesh_rid;
    godot::RID instance_rid;
    godot::RID shader_rid;
    godot::RID material_rid;
    int num_voxels;
};
