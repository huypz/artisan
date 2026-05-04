#ifndef TILE_CHUNK_H
#define TILE_CHUNK_H

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/box_mesh.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/shader.hpp>
#include "godot_cpp/classes/shader_material.hpp"

struct TileCellData {
    std::array<int, 32> tiles;
};

class TileChunk : public godot::Node3D {
    GDCLASS(TileChunk, godot::Node3D)

private:
    int chunk_size;
    godot::LocalVector<TileCellData> cells;
    godot::RID multimesh_rid;
    godot::RID instance_rid;

    godot::Ref<godot::ArrayMesh> mesh;
    godot::Ref<godot::Shader> shader;
    godot::Ref<godot::ShaderMaterial> material;

    godot::Array surface_array;
    godot::PackedVector3Array vertices;
    godot::PackedVector3Array normals;
    godot::PackedColorArray colors;

    godot::PackedVector3Array cube_vertices = {
        godot::Vector3(-0.5f, -0.5f,  0.5f),
        godot::Vector3( 0.5f, -0.5f,  0.5f),
        godot::Vector3( 0.5f, -0.5f, -0.5f),
        godot::Vector3(-0.5f, -0.5f, -0.5f),
        godot::Vector3(-0.5f,  0.5f,  0.5f),
        godot::Vector3( 0.5f,  0.5f,  0.5f),
        godot::Vector3( 0.5f,  0.5f, -0.5f),
        godot::Vector3(-0.5f,  0.5f, -0.5f),
    };

    enum Face : uint8_t { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };

    std::array<int, 36> face_indices = {
        1, 5, 6, 1, 6, 2,   // RIGHT
        3, 7, 4, 3, 4, 0,   // LEFT
        4, 7, 6, 4, 6, 5,   // TOP
        3, 0, 1, 3, 1, 2,   // BOTTOM
        0, 4, 5, 0, 5, 1,   // FRONT
        2, 6, 7, 2, 7, 3    // BACK
    };

    godot::PackedVector3Array face_normals = {
        godot::Vector3( 1,  0,  0),
        godot::Vector3(-1,  0,  0),
        godot::Vector3( 0,  1,  0),
        godot::Vector3( 0, -1,  0),
        godot::Vector3( 0,  0,  1),
        godot::Vector3( 0,  0, -1)
    };

    godot::PackedColorArray face_colors = {
        godot::Color{ 1.0f, 0.0f, 0.0f, 1.0f },
        godot::Color{ 1.0f, 1.0f, 0.0f, 1.0f },
        godot::Color{ 0.0f, 1.0f, 0.0f, 1.0f },
        godot::Color{ 0.0f, 1.0f, 1.0f, 1.0f },
        godot::Color{ 0.0f, 0.0f, 1.0f, 1.0f },
        godot::Color{ 1.0f, 0.0f, 1.0f, 1.0f }
    };

    void add_face(Face face);

    void generate();

    void cleanup();

protected:
    static void _bind_methods();

public:
    TileChunk();
    ~TileChunk();
    void _enter_tree() override;
    void _exit_tree() override;
    void _notification(int p_what);
};

#endif
