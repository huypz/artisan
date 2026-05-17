#include "voxel_grid.h"

#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/core/math.hpp>

#include "voxel/voxel_metrics.h"
#include "voxel_chunk.h"

using namespace godot;

void VoxelGrid::_bind_methods() {
    // Chunk
    ClassDB::bind_method(D_METHOD("set_chunk_count_x", "value"), &VoxelGrid::set_chunk_count_x);
    ClassDB::bind_method(D_METHOD("get_chunk_count_x"),          &VoxelGrid::get_chunk_count_x);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "chunk_count_x"), "set_chunk_count_x", "get_chunk_count_x");
    ClassDB::bind_method(D_METHOD("set_chunk_count_y", "value"), &VoxelGrid::set_chunk_count_y);
    ClassDB::bind_method(D_METHOD("get_chunk_count_y"),          &VoxelGrid::get_chunk_count_y);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "chunk_count_y"), "set_chunk_count_y", "get_chunk_count_y");
    ClassDB::bind_method(D_METHOD("set_chunk_count_z", "value"), &VoxelGrid::set_chunk_count_z);
    ClassDB::bind_method(D_METHOD("get_chunk_count_z"),          &VoxelGrid::get_chunk_count_z);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "chunk_count_z"), "set_chunk_count_z", "get_chunk_count_z");

    // Editor
    ClassDB::bind_method(D_METHOD("set_brush_texture", "value"), &VoxelGrid::set_brush_texture);
    ClassDB::bind_method(D_METHOD("get_brush_texture"),          &VoxelGrid::get_brush_texture);
    ADD_PROPERTY(PropertyInfo(Variant::INT, "brush_texture"), "set_brush_texture", "get_brush_texture");
}

VoxelGrid::VoxelGrid() {
    chunk_count_x = 1;
    chunk_count_y = 1;
    chunk_count_z = 1;
    brush_texture = 1;
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
            paint_face(hit_result, brush_texture);
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
}

void VoxelGrid::free() {
    for (VoxelChunk* chunk : chunks) {
        if (chunk) {
            chunk->queue_free();
        }
    }
    chunks.clear();
}

VoxelChunk* VoxelGrid::get_chunk(Vector3 position) {
    if (position.x < 0 || position.y < 0 || position.z < 0) {
        return nullptr;
    }

    int chunk_x = (int)position.x / VoxelMetrics::CHUNK_SIZE_X;
    int chunk_y = (int)position.y / VoxelMetrics::CHUNK_SIZE_Y;
    int chunk_z = (int)position.z / VoxelMetrics::CHUNK_SIZE_Z;

    if (chunk_x >= chunk_count_x ||
        chunk_y >= chunk_count_y ||
        chunk_z >= chunk_count_z) {
        return nullptr;
    }

    return chunks[chunk_x + chunk_y * chunk_count_x + chunk_z * chunk_count_x * chunk_count_y];
}

Voxel* VoxelGrid::get_voxel(Vector3 position) {
    VoxelChunk* chunk = get_chunk(position);
    if (!chunk) {
        return nullptr;
    }

    int local_x = (int)Math::floor(position.x) % VoxelMetrics::CHUNK_SIZE_X;
    int local_y = (int)Math::floor(position.y) % VoxelMetrics::CHUNK_SIZE_Y;
    int local_z = (int)Math::floor(position.z) % VoxelMetrics::CHUNK_SIZE_Z;

    return chunk->get_voxel(local_x, local_y, local_z);
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

void VoxelGrid::set_chunk_count_y(int value) {
    chunk_count_y = value;
    if (is_inside_tree()) {
        free();
        build();
    }
}

int VoxelGrid::get_chunk_count_y() const {
    return chunk_count_y;
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
    chunks.resize(chunk_count_x * chunk_count_y * chunk_count_z);

    for (int z = 0; z < chunk_count_z; z++) {
        for (int y = 0; y < chunk_count_y; y++) {
            for (int x = 0; x < chunk_count_x; x++) {
                int i = x + y * chunk_count_x + z * chunk_count_x * chunk_count_y;
                VoxelChunk *chunk = chunks[i] = memnew(VoxelChunk);
                chunk->set_name(vformat("Chunk_%d_%d_%d", x, y, z));
                chunk->set_position(Vector3(
                    x * VoxelMetrics::CHUNK_SIZE_X,
                    y * VoxelMetrics::CHUNK_SIZE_Y,
                    z * VoxelMetrics::CHUNK_SIZE_Z
                ));
                add_child(chunk);

                chunk->build_mesh();
            }
        }
    }
}

#pragma endregion

#pragma region Editor

void VoxelGrid::paint_face(const VoxelHit& hit, int texture) {
    Voxel new_voxel = *hit.voxel;
    new_voxel.face_tex[hit.face] = texture;

    VoxelChunk* chunk = get_chunk(hit.voxel_pos);
    int local_index = chunk->get_index(hit.voxel_pos.x, hit.voxel_pos.y, hit.voxel_pos.z);
    chunk->set_voxel(local_index, new_voxel);
}

void VoxelGrid::set_brush_texture(int value) {
    brush_texture = value;
}

int VoxelGrid::get_brush_texture() const {
    return brush_texture;
}

bool VoxelGrid::raycast_voxel(Vector3 origin, Vector3 direction, VoxelHit& hit_result) {
    origin = to_local(origin);

    Vector3i pos(
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
        ((step.x > 0 ? pos.x + 1 : pos.x) - origin.x) / direction.x,
        ((step.y > 0 ? pos.y + 1 : pos.y) - origin.y) / direction.y,
        ((step.z > 0 ? pos.z + 1 : pos.z) - origin.z) / direction.z
    );

    // edge case check - camera inside voxel
    float t_hit;
    Face face;
    auto step_dda = [&]() {
        if (t_max.x < t_max.y && t_max.x < t_max.z) {
            t_hit = t_max.x;
            pos.x += step.x;
            t_max.x += t_delta.x;
            face = step.x > 0 ? Face::LEFT : Face::RIGHT;
        } else if (t_max.y < t_max.z) {
            t_hit = t_max.y;
            pos.y += step.y;
            t_max.y += t_delta.y;
            face = step.y > 0 ? Face::BOTTOM : Face::TOP;
        } else {
            t_hit = t_max.z;
            pos.z += step.z;
            t_max.z += t_delta.z;
            face = step.z > 0 ? Face::BACK : Face::FRONT;
        }
    };
    step_dda(); // first step - skips starting voxel, face now valid

    const int MAX_STEPS = 50;
    for (int i = 0; i < MAX_STEPS; i++) {
        Voxel* voxel = get_voxel(pos);
        if (voxel && voxel->type != 0) {
            hit_result.voxel     = voxel;
            hit_result.voxel_pos = pos;
            hit_result.face      = face;
            hit_result.hit_pos   = origin + direction * t_hit;
            return true;
        }
        step_dda();
    }

    return false;
}

#pragma endregion
