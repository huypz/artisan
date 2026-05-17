#include "voxel_grid.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/worker_thread_pool.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

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

}

void VoxelGrid::_input(const Ref<InputEvent>& event) {
    Ref<InputEventMouseButton> mouse = event;
    if (mouse.is_null()) return;

    if (mouse->get_button_index() == MOUSE_BUTTON_LEFT && mouse->is_pressed()) {
        Camera3D* camera = get_viewport()->get_camera_3d();
        if (camera == nullptr) return;

        Vector2 mouse_pos = get_viewport()->get_mouse_position();
        Vector3 ray_origin = camera->project_ray_origin(mouse_pos);
        Vector3 ray_dir    = camera->project_ray_normal(mouse_pos);

        VoxelHit hit_result;
        if (raycast_voxel(ray_origin, ray_dir, hit_result)) {
            UtilityFunctions::print("hit voxel at ", hit_result.hit_pos, " and face ", hit_result.face);

        }
    }
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

Voxel* VoxelGrid::get_voxel(Vector3 position) {
    if (position.x < 0 || position.z < 0) {
        return nullptr;
    }

    int chunk_x = (int)Math::floor(position.x) / VoxelMetrics::CHUNK_SIZE_X;
    int chunk_z = (int)Math::floor(position.z) / VoxelMetrics::CHUNK_SIZE_Z;
    if (chunk_x < 0 || chunk_x >= chunk_count_x ||
        chunk_z < 0 || chunk_z >= chunk_count_z) {
        return nullptr;
    }
    VoxelChunk* chunk = chunks[chunk_x + chunk_z * chunk_count_x];
    if (chunk == nullptr) {
        return nullptr;
    }
    int local_x = (int)Math::floor(position.x) % VoxelMetrics::CHUNK_SIZE_X;
    int local_z = (int)Math::floor(position.z) % VoxelMetrics::CHUNK_SIZE_Z;

    int y = Math::floor(position.y);
    if (y < 0 || y >= VoxelMetrics::CHUNK_SIZE_Y) {
        return nullptr;
    }
    VoxelColumn* column = chunk->get_column(local_x, local_z);
    if (column == nullptr) {
        return nullptr;
    }

    return &(*column)[y];
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
        0.5f,
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

bool VoxelGrid::raycast_voxel(Vector3 origin, Vector3 direction, VoxelHit& hit_result) {
    origin = to_local(origin);

    Vector3i voxel_pos(
        Math::floor(origin.x),
        Math::floor(origin.y),
        Math::floor(origin.z)
    );

    Vector3i step(
        direction.x > 0 ? 1 : -1,
        direction.y > 0 ? 1 : -1,
        direction.z > 0 ? 1 : -1
    );

    Vector3 t_delta(
        Math::abs(1.0f / direction.x),
        Math::abs(1.0f / direction.y),
        Math::abs(1.0f / direction.z)
    );

    Vector3 t_max(
        ((step.x > 0 ? voxel_pos.x + 1 : voxel_pos.x) - origin.x) / direction.x,
        ((step.y > 0 ? voxel_pos.y + 1 : voxel_pos.y) - origin.y) / direction.y,
        ((step.z > 0 ? voxel_pos.z + 1 : voxel_pos.z) - origin.z) / direction.z
    );

    float t_hit = 0.0f;
    Face face;
    const int MAX_STEPS = 20;
    for (int i = 0; i < MAX_STEPS; i++) {
        Voxel* voxel = get_voxel(voxel_pos);
        if (voxel && voxel->type) {
            hit_result.voxel = voxel;
            hit_result.voxel_pos = voxel_pos;
            hit_result.face = face;
            hit_result.hit_pos = origin + direction * t_hit;
            return true;
        }

        if (t_max.x < t_max.y && t_max.x < t_max.z) {
            t_hit = t_max.x;
            voxel_pos.x += step.x;
            t_max.x += t_delta.x;
            face = step.x > 0 ? Face::LEFT : Face::RIGHT;
        } else if (t_max.y < t_max.z) {
            t_hit = t_max.y;
            voxel_pos.y += step.y;
            t_max.y += t_delta.y;
            face = step.y > 0 ? Face::BOTTOM : Face::TOP;
        } else {
            t_hit = t_max.z;
            voxel_pos.z += step.z;
            t_max.z += t_delta.z;
            face = step.z > 0 ? Face::BACK : Face::FRONT;
        }
    }

    return false;
}

#pragma endregion
