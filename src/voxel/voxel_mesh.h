#pragma once

#include "godot_cpp/variant/vector3.hpp"
#include <godot_cpp/classes/array_mesh.hpp>

enum Face : uint8_t { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };

class VoxelMesh {

private:
    godot::Ref<godot::ArrayMesh> mesh;

    godot::Array surface_array;
    godot::PackedVector3Array vertices;
    godot::PackedVector3Array normals;
    godot::PackedVector2Array uvs;

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

    godot::Array face_indices = {
        1, 5, 6, 1, 6, 2,   // RIGHT
        3, 7, 4, 3, 4, 0,   // LEFT
        4, 7, 6, 4, 6, 5,   // TOP
        3, 0, 1, 3, 1, 2,   // BOTTOM
        0, 4, 5, 0, 5, 1,   // FRONT
        2, 6, 7, 2, 7, 3,    // BACK
    };

    godot::PackedVector3Array face_normals = {
        godot::Vector3( 1,  0,  0),
        godot::Vector3(-1,  0,  0),
        godot::Vector3( 0,  1,  0),
        godot::Vector3( 0, -1,  0),
        godot::Vector3( 0,  0,  1),
        godot::Vector3( 0,  0, -1),
    };

    godot::PackedVector2Array face_uvs = {
        godot::Vector2(0, 1),
        godot::Vector2(0, 0),
        godot::Vector2(1, 0),
        godot::Vector2(0, 1),
        godot::Vector2(1, 0),
        godot::Vector2(1, 1),
    };

public:
    void add_face(Face face);
    void generate_mesh();

    godot::RID get_rid() const;
};
