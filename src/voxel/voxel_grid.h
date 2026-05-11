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

    void _process(float delta);

    void _enter_tree() override;
    void _exit_tree() override;

    void build();
    void free();

    void  set_chunk_count_x(int value);
    int   get_chunk_count_x() const;
    void  set_chunk_count_z(int value);
    int   get_chunk_count_z() const;
};
