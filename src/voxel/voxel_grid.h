#pragma once

#include <godot_cpp/classes/node3d.hpp>

class Voxel;
class VoxelChunk;
class VoxelColumn;

class VoxelGrid : public godot::Node3D {
    GDCLASS(VoxelGrid, godot::Node3D)

protected:
    static void _bind_methods();

public:
    VoxelGrid();
    void _process(double delta) override;
    void _enter_tree() override;
    void _exit_tree() override;

#pragma region Chunk

public:
    void build();
    void free();
    void  set_chunk_count_x(int value);
    int   get_chunk_count_x() const;
    void  set_chunk_count_z(int value);
    int   get_chunk_count_z() const;

private:
    int chunk_count_x;
    int chunk_count_z;
    godot::LocalVector<VoxelChunk*> chunks;
    void create_chunks();
    void create_column(int x, int z, int i);
    void create_columns();

#pragma endregion

#pragma region Editor

private:
    void handle_input();

#pragma endregion
};
