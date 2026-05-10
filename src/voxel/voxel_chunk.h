#pragma once

#include <godot_cpp/classes/node3d.hpp>

#include "voxel/voxel_mesh.h"

class VoxelColumn;

class VoxelChunk : public godot::Node3D {
    GDCLASS(VoxelChunk, godot::Node3D)

private:
    godot::LocalVector<VoxelColumn*> columns;

    VoxelMesh mesh;
    godot::RID multimesh_rid;
    godot::RID instance_rid;

    godot::PackedFloat32Array buffer;

protected:
    static void _bind_methods();

public:
    void _enter_tree() override;
    void _exit_tree() override;

    void assign_column(int index, VoxelColumn* column);

    void build_mesh();
    void buffer_mesh();
    void free_mesh();
    void rebuild_mesh();
};
