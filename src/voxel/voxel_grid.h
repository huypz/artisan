#pragma once

#include <godot_cpp/classes/node3d.hpp>

class Voxel;
class VoxelChunk;
class VoxelColumn;

class VoxelGrid : public godot::Node3D {
    GDCLASS(VoxelGrid, godot::Node3D)

private:
    int chunk_count_x;
    int chunk_count_z;

    godot::LocalVector<VoxelChunk*> chunks;

    void create_chunks();
    void create_column(int x, int z, int i);
    void create_columns();

protected:
    static void _bind_methods();

public:
    VoxelGrid();
    void _notification(int p_what);
    void _enter_tree() override;
    void _exit_tree() override;

    void build();
    void free();

    VoxelChunk* get_chunk(int x, int z) const;
    VoxelColumn* get_column(int x, int z) const;
    Voxel* get_voxel(int x, int y, int z) const;
};
