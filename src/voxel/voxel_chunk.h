#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

#include "voxel/voxel_metrics.h"
#include "voxel.h"

class VoxelColumn;

class VoxelChunk : public godot::Node3D {
    GDCLASS(VoxelChunk, godot::Node3D)

protected:
    static void _bind_methods();

public:
    void _enter_tree() override;
    void _exit_tree() override;

    void init();
    void build_mesh();
    void free_mesh();

    int get_index(int x, int y, int z);
    void set_voxel(int index, Voxel new_voxel);
    Voxel* get_voxel(int x, int y, int z);

private:
    static constexpr int NUM_VOXELS = VoxelMetrics::CHUNK_SIZE_X *
                                      VoxelMetrics::CHUNK_SIZE_Y *
                                      VoxelMetrics::CHUNK_SIZE_Z;
    Voxel voxels[NUM_VOXELS];
    godot::Ref<godot::ArrayMesh> mesh;
    godot::RID instance_rid;
    godot::RID shader_rid;
    godot::RID material_rid;
};
