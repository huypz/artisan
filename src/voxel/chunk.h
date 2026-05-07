#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/shader.hpp>
#include <godot_cpp/classes/texture2d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>

inline constexpr int CHUNK_SIZE = 8;

struct ChunkData {
    godot::LocalVector<uint8_t> voxel_id;

    void resize(int size) {
        voxel_id.resize(size);
        memset(voxel_id.ptr(), 0, size * sizeof(uint8_t));
    }

    static inline int index(int x, int y, int z) {
        return x + CHUNK_SIZE * (z + CHUNK_SIZE * y);
    }

    uint8_t get(int x, int y, int z) const { return voxel_id[index(x, y, z)]; }
    void set(int x, int y, int z, uint8_t new_voxel_type) { voxel_id[index(x, y, z)] = new_voxel_type; }
};

class Chunk : public godot::Node3D {
    GDCLASS(Chunk, godot::Node3D)

    enum Face : uint8_t { RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK };

    static inline int index(int x, int y, int z) {
        return x + CHUNK_SIZE * (z + CHUNK_SIZE * y);
    }

private:
    ChunkData data;

    godot::RID multimesh_rid;
    godot::RID instance_rid;

    godot::Ref<godot::ArrayMesh> mesh;
    godot::Ref<godot::Shader> shader;
    godot::Ref<godot::ShaderMaterial> material;

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

    void add_face(Face face);

    void generate_mesh();

    void generate_chunk();

    void cleanup();

protected:
    static void _bind_methods();

public:
    ~Chunk();
    void _enter_tree() override;
    void _exit_tree() override;
    void _notification(int p_what);
};
