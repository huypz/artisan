#pragma once

#include <cstdint>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/input_event.hpp>

class VoxelChunk;
class VoxelColumn;
struct Voxel;

enum Face : uint8_t { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };

class VoxelGrid : public godot::Node3D {
    GDCLASS(VoxelGrid, godot::Node3D)

protected:
    static void _bind_methods();

public:
    VoxelGrid();
    void _process(double delta) override;
    void _input(const godot::Ref<godot::InputEvent>& event) override;
    void _enter_tree() override;
    void _exit_tree() override;

#pragma region Chunk

public:
    void build();
    void free();

    VoxelChunk* get_chunk(godot::Vector3 position);
    Voxel* get_voxel(godot::Vector3 position);

    void  set_chunk_count_x(int value);
    int   get_chunk_count_x() const;
    void  set_chunk_count_y(int value);
    int   get_chunk_count_y() const;
    void  set_chunk_count_z(int value);
    int   get_chunk_count_z() const;

private:
    int chunk_count_x;
    int chunk_count_y;
    int chunk_count_z;
    godot::LocalVector<VoxelChunk*> chunks;
    void create_chunks();

#pragma endregion

#pragma region Editor

private:
    struct VoxelHit {
        Voxel* voxel;
        godot::Vector3i voxel_pos;
        godot::Vector3 hit_pos;
        Face face;
    };

public:
    void paint_face(const VoxelHit& hit, int texture);
    void set_brush_tile_id(int value);
    int  get_brush_tile_id() const;

private:
    int brush_tile_id;
    bool raycast_voxel(godot::Vector3 origin, godot::Vector3 direction, VoxelHit& hit_result);

#pragma endregion
};
