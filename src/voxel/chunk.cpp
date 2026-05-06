#include "chunk.h"

#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void Chunk::_bind_methods() {}

Chunk::~Chunk() {
    cleanup();
}

void Chunk::_enter_tree() {
    generate_chunk();
}

void Chunk::_exit_tree() {
    cleanup();
}

void Chunk::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_EXTENSION_RELOADED:
            cleanup();
            generate_chunk();
            break;
    }
}

void Chunk::add_face(Face face) {
    for (int i = 0; i < 6; i++) {
        int vi = face_indices[face * 6 + i];
        vertices.push_back(cube_vertices[vi]);
        normals.push_back(face_normals[face]);
        uvs.push_back(face_uvs[i]);
    }
}

void Chunk::generate_mesh() {
    surface_array.resize(Mesh::ARRAY_MAX);

    add_face(Face::RIGHT);
    add_face(Face::LEFT);
    add_face(Face::TOP);
    add_face(Face::BOTTOM);
    add_face(Face::FRONT);
    add_face(Face::BACK);

    surface_array[Mesh::ARRAY_VERTEX] = vertices;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_TEX_UV] = uvs;

    mesh.instantiate();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    shader = ResourceLoader::get_singleton()->load("res://shaders/voxel_shader.gdshader");
    material.instantiate();
    material->set_shader(shader);

    mesh->surface_set_material(0, material);
}

void Chunk::generate_chunk() {
    LocalVector<LocalVector<int>> chunk_data = {
        {1,1},  {1},  {1},  {1},   {1},   {1},   {1},  {1,1},
        {1},    {1},  {1},  {1},   {1},   {1},   {1},  {1},
        {1},    {1},  {1},  {1},   {1},   {1},   {1},  {1},
        {1},    {1},  {1},  {1,1,2}, {1},   {1,1}, {1},  {1},
        {1},    {1},  {1},  {1,1}, {1},   {1,1}, {1},  {1},
        {1},    {1},  {1},  {1,1}, {1,1}, {1,1}, {1},  {1},
        {1},    {1},  {1},  {1},   {1},   {1},   {1},  {1},
        {1},    {1},  {1},  {1},   {1},   {1},   {1,1},{1,1},
    };

    int CHUNK_VOLUME = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;
    data.resize(CHUNK_VOLUME);
    int num_blocks = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            int col = x + CHUNK_SIZE * z;
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (y < chunk_data[col].size()) {
                    data.set(x, y, z, chunk_data[col][y]);
                    num_blocks++;
                }
                else {
                    data.set(x, y, z, 0);
                }
            }
        }
    }

    if (mesh.is_null()) {
        generate_mesh();
    }

    RenderingServer *rs = RenderingServer::get_singleton();
    multimesh_rid = rs->multimesh_create();
    rs->multimesh_set_mesh(multimesh_rid, mesh->get_rid());
    rs->multimesh_allocate_data(
        multimesh_rid,
        num_blocks,
        RenderingServer::MULTIMESH_TRANSFORM_3D,
        false,   // use_colors
        true    // use_custom_data
    );

    // Transform3D(12) + Colors(4) + Custom(4)
    const uint8_t stride = 16;
    PackedFloat32Array buffer;
    buffer.resize(num_blocks * stride);
    float *p = buffer.ptrw();

    int block = 0;
    for (int z = 0; z < CHUNK_SIZE; z++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_SIZE; y++) {
                if (data.get(x, y, z) == 0) continue;
                float *q = p + (block * stride);
                q[0] = 1.0f; q[1] = 0.0f; q[2]  = 0.0f; q[3]  = x + 0.5f;
                q[4] = 0.0f; q[5] = 1.0f; q[6]  = 0.0f; q[7]  = y - 0.5f;
                q[8] = 0.0f; q[9] = 0.0f; q[10] = 1.0f; q[11] = z + 0.5f;

                // CUSTOM_DATA
                q[12] = data.get(x, y, z);  // voxel_id
                q[13] = 0.0f;
                q[14] = 0.0f;
                q[15] = 0.0f;

                block++;
            }
        }
    }

    rs->multimesh_set_buffer(multimesh_rid, buffer);
    instance_rid = rs->instance_create2(multimesh_rid, get_world_3d()->get_scenario());
    rs->instance_set_transform(instance_rid, get_global_transform());
}

void Chunk::cleanup() {
    RenderingServer *rs = RenderingServer::get_singleton();
    rs->free_rid(instance_rid);
    rs->free_rid(multimesh_rid);

    vertices.clear();
    normals.clear();
    surface_array.clear();
}
