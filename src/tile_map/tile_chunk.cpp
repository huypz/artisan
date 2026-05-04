#include "tile_chunk.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

void TileChunk::_bind_methods() {

}

TileChunk::TileChunk() {
    chunk_size = 8;
}

TileChunk::~TileChunk() {
    cleanup();
}

void TileChunk::_enter_tree() {
    generate_chunk();
}

void TileChunk::_exit_tree() {
    cleanup();
}

void TileChunk::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_EXTENSION_RELOADED:
            cleanup();
            generate_chunk();
            break;
    }
}

void TileChunk::add_face(Face face) {
    for (int t = 0; t < 2; t++) {
        for (int v = 0; v < 3; v++) {
            int i = face_indices[face*6 + t*3 + v];
            vertices.push_back(cube_vertices[i]);
            normals.push_back(face_normals[face]);
            colors.push_back(face_colors[face]);
        }
    }
}

void TileChunk::generate_mesh() {
    surface_array.resize(Mesh::ARRAY_MAX);

    add_face(Face::RIGHT);
    add_face(Face::LEFT);
    add_face(Face::TOP);
    add_face(Face::BOTTOM);
    add_face(Face::FRONT);
    add_face(Face::BACK);

    surface_array[Mesh::ARRAY_VERTEX] = vertices;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_COLOR] = colors;

    mesh.instantiate();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);

    shader = ResourceLoader::get_singleton()->load("res://shaders/tile.gdshader");
    material.instantiate();
    material->set_shader(shader);
    mesh->surface_set_material(0, material);
}

void TileChunk::generate_chunk() {
    if (mesh.is_null()) {
        generate_mesh();
    }

    int num_blocks = 0;
    for (int i = 0; i < chunk_size * chunk_size; i++) {
        tiles.push_back({ { 1, 0, 1 } });
    }
    for (int i = 0; i < tiles.size(); i++) {
        num_blocks += tiles[i].blocks.size();
    }

    RenderingServer *rs = RenderingServer::get_singleton();
    multimesh_rid = rs->multimesh_create();
    rs->multimesh_set_mesh(multimesh_rid, mesh->get_rid());
    rs->multimesh_allocate_data(
        multimesh_rid,
        num_blocks,
        RenderingServer::MULTIMESH_TRANSFORM_3D,
        false,   // use_colors
        false    // use_custom_data
    );

    UtilityFunctions::print("num_blocks", num_blocks);

    const uint8_t stride = 12;
    PackedFloat32Array buffer;
    buffer.resize(num_blocks * stride);
    float *p = buffer.ptrw();

    int block = 0;
    int tile = 0;
    for (int x = 0; x < chunk_size; x++) {
        for (int z = 0; z < chunk_size; z++) {
            for (int y = 0; y < tiles[tile].blocks.size(); y++) {
                float *q = p + (block * stride);
                q[0] = 1.0f; q[1] = 0.0f; q[2]  = 0.0f; q[3]  = x + 0.5f;
                q[4] = 0.0f; q[5] = 1.0f; q[6]  = 0.0f; q[7]  = y - 0.5f;
                q[8] = 0.0f; q[9] = 0.0f; q[10] = 1.0f; q[11] = z + 0.5f;
                block++;
            }
            tile++;
        }
    }

    rs->multimesh_set_buffer(multimesh_rid, buffer);
    instance_rid = rs->instance_create2(multimesh_rid, get_world_3d()->get_scenario());
    rs->instance_set_transform(instance_rid, get_global_transform());
}

void TileChunk::cleanup() {
    RenderingServer *rs = RenderingServer::get_singleton();
    rs->free_rid(instance_rid);
    rs->free_rid(multimesh_rid);
    tiles.clear();

    vertices.clear();
    normals.clear();
    colors.clear();
    surface_array.clear();
}
