#include "voxel_grid.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>

#include "godot_cpp/variant/utility_functions.hpp"
#include "voxel/voxel_column.h"
#include "voxel/voxel_metrics.h"
#include "voxel_chunk.h"

using namespace godot;

void VoxelGrid::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_chunk_count_x", "value"), &VoxelGrid::set_chunk_count_x);
    ClassDB::bind_method(D_METHOD("get_chunk_count_x"),          &VoxelGrid::get_chunk_count_x);
    ClassDB::bind_method(D_METHOD("set_chunk_count_z", "value"), &VoxelGrid::set_chunk_count_z);
    ClassDB::bind_method(D_METHOD("get_chunk_count_z"),          &VoxelGrid::get_chunk_count_z);

    ADD_PROPERTY(PropertyInfo(Variant::INT, "chunk_count_x"), "set_chunk_count_x", "get_chunk_count_x");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "chunk_count_z"), "set_chunk_count_z", "get_chunk_count_z");
}

VoxelGrid::VoxelGrid() {
    chunk_count_x = 1;
    chunk_count_z = 1;
}

void VoxelGrid::_process(double delta) {
    handle_input();
}

void VoxelGrid::_enter_tree() {
    build();
}

void VoxelGrid::_exit_tree() {
    free();
}

#pragma region Chunk

void VoxelGrid::build() {
    ERR_FAIL_COND_MSG(
        chunk_count_x <= 0 || chunk_count_z <= 0,
        "chunk_count_x and chunk_count_z must be greater than 0"
    );

    create_chunks();
    create_columns();

    for (int i = 0; i < chunks.size(); i++) {
        WorkerThreadPool::get_singleton()->add_task(
            callable_mp(chunks[i], &VoxelChunk::build_buffer)
        );
    }
}

void VoxelGrid::free() {
    for (VoxelChunk* chunk : chunks) {
        if (chunk) {
            chunk->queue_free();
        }
    }
    chunks.clear();
}

void VoxelGrid::set_chunk_count_x(int value) {
    chunk_count_x = value;
    if (is_inside_tree()) {
        free();
        build();
    }
}

int VoxelGrid::get_chunk_count_x() const {
    return chunk_count_x;
}

void VoxelGrid::set_chunk_count_z(int value) {
    chunk_count_z = value;
    if (is_inside_tree()) {
        free();
        build();
    }
}

int VoxelGrid::get_chunk_count_z() const {
    return chunk_count_z;
}

void VoxelGrid::create_chunks() {
    chunks.resize(chunk_count_x * chunk_count_z);

    for (int z = 0, i = 0; z < chunk_count_z; z++) {
        for (int x = 0; x < chunk_count_x; x++, i++) {
            VoxelChunk *chunk = chunks[i] = memnew(VoxelChunk);
            chunk->set_name(vformat("Chunk_%d_%d", x, z));
            add_child(chunk);
        }
    }
}

void VoxelGrid::create_column(int x, int z, int i) {
    Vector3 position(
        x * VoxelMetrics::LENGTH_X + 0.5f,
        -0.5f,
        z * VoxelMetrics::LENGTH_Z + 0.5f
    );
    VoxelColumn* column = memnew(VoxelColumn);
    column->world_x = position.x;
    column->world_y = position.y;
    column->world_z = position.z;
    // assign column to chunk
    int chunk_x = position.x / VoxelMetrics::CHUNK_SIZE_X;
    int chunk_z = position.z / VoxelMetrics::CHUNK_SIZE_Z;
    VoxelChunk *chunk = chunks[chunk_x + chunk_z * chunk_count_x];
    int local_x = x - chunk_x * VoxelMetrics::CHUNK_SIZE_X;
    int local_z = z - chunk_z * VoxelMetrics::CHUNK_SIZE_Z;
    chunk->assign_column(local_x + local_z * VoxelMetrics::CHUNK_SIZE_X, column);
}

void VoxelGrid::create_columns() {
    int column_count_x = chunk_count_x * VoxelMetrics::CHUNK_SIZE_X;
    int column_count_z = chunk_count_z * VoxelMetrics::CHUNK_SIZE_Z;
    for (int z = 0; z < column_count_z; z++) {
        for (int x = 0; x < column_count_x; x++) {
            create_column(x, z, x + z * column_count_x);
        }
    }
}

#pragma endregion

#pragma region Editor

void VoxelGrid::handle_input() {
    if (Input::get_singleton()->is_mouse_button_pressed(MOUSE_BUTTON_LEFT)) {
        Camera3D* camera = get_viewport()->get_camera_3d();
        if (camera == nullptr) return;
        Vector2 mouse_pos = get_viewport()->get_mouse_position();

        Vector3 ray_origin = camera->project_ray_origin(mouse_pos);
        Vector3 ray_dir    = camera->project_ray_normal(mouse_pos);

        // intersect with Y=0 plane — no physics/collision needed
        if (ray_dir.y != 0.0f) {
            float t = -ray_origin.y / ray_dir.y;
            if (t > 0.0f) {
                Vector3 world_pos = ray_origin + ray_dir * t;

                UtilityFunctions::print("touched at ", world_pos);
            }
        }
    }
}

#pragma endregion
