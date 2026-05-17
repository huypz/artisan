#include "voxel_chunk.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>

#include "voxel/voxel_metrics.h"
#include "voxel_mesh.h"

using namespace godot;

void VoxelChunk::_bind_methods() {}

void VoxelChunk::_enter_tree() {
    init();
    build_mesh();
}

void VoxelChunk::_exit_tree() {
    free_mesh();
}

void VoxelChunk::init() {
    for (int z = 0; z < VoxelMetrics::CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < VoxelMetrics::CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < VoxelMetrics::CHUNK_SIZE_X; x++) {
                int i = get_index(x, y, z);
                voxels[i].type = y == 0 ? 1 : 0;
            }
        }
    }
}

void VoxelChunk::build_mesh() {
    VoxelMesh builder;
    builder.build(voxels);
    mesh = builder.get_mesh();
    if (mesh.is_null()) {
        return;
    }

    RenderingServer* rs = RenderingServer::get_singleton();
    instance_rid = rs->instance_create2(mesh->get_rid(), get_world_3d()->get_scenario());
    rs->instance_set_transform(instance_rid, get_global_transform());

    shader_rid = rs->shader_create();
    rs->shader_set_code(shader_rid, R"(
        shader_type spatial;
        render_mode unshaded, cull_back;

        global uniform sampler2D voxel_atlas : source_color, filter_nearest;

        void fragment() {
            ALBEDO = texture(voxel_atlas, UV).rgb;
        }
    )");
    material_rid = rs->material_create();
    rs->material_set_shader(material_rid, shader_rid);
    rs->instance_geometry_set_material_override(instance_rid, material_rid);
}

void VoxelChunk::free_mesh() {
    RenderingServer *rs = RenderingServer::get_singleton();
    if (instance_rid.is_valid()) rs->free_rid(instance_rid);
    if (material_rid.is_valid()) rs->free_rid(material_rid);
    if (shader_rid.is_valid()) rs->free_rid(shader_rid);
    instance_rid = RID();
    material_rid = RID();
    shader_rid = RID();
    mesh.unref();
}

int VoxelChunk::get_index(int x, int y, int z) {
    x = x % VoxelMetrics::CHUNK_SIZE_X;
    y = y % VoxelMetrics::CHUNK_SIZE_Y;
    z = z % VoxelMetrics::CHUNK_SIZE_Z;

    return x + y * VoxelMetrics::CHUNK_SIZE_X + z * VoxelMetrics::CHUNK_SIZE_X * VoxelMetrics::CHUNK_SIZE_Y;
}

void VoxelChunk::set_voxel(int index, Voxel new_voxel) {
    ERR_FAIL_COND_MSG(index < 0 || index >= NUM_VOXELS, "VoxelChunk::set_voxel - index is out of bounds");

   voxels[index] = new_voxel;
   free_mesh();
   build_mesh();
}

Voxel* VoxelChunk::get_voxel(int x, int y, int z) {
    int i = get_index(x, y, z);
    int num_voxels = VoxelMetrics::CHUNK_SIZE_X * VoxelMetrics::CHUNK_SIZE_Y * VoxelMetrics::CHUNK_SIZE_Z;
    if (i < 0 || i >= num_voxels) {
        return nullptr;
    }

    return &voxels[i];
}
