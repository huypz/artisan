#include "voxel_chunk.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>

#include "voxel/voxel_column.h"
#include "voxel/voxel_metrics.h"

using namespace godot;

void VoxelChunk::_bind_methods() {}

void VoxelChunk::_enter_tree() {
    columns.resize(VoxelMetrics::CHUNK_SIZE_X * VoxelMetrics::CHUNK_SIZE_Z);
    mesh.add_face(Face::TOP);
    mesh.generate_mesh();
}

void VoxelChunk::_exit_tree() {
    free_mesh();
}

void VoxelChunk::assign_column(int index, VoxelColumn* column) {
    columns[index] = column;
}

void VoxelChunk::build_buffer() {
    const uint8_t stride = 16;
    buffer.resize(columns.size() * stride);
    float *p = buffer.ptrw();
    for (int z = 0, i = 0; z < VoxelMetrics::CHUNK_SIZE_Z; z++) {
        for (int x = 0; x < VoxelMetrics::CHUNK_SIZE_X; x++) {
            VoxelColumn* column = columns[x + z * VoxelMetrics::CHUNK_SIZE_X];
            float *q = p + (i++ * stride);
            q[0]  = 1.0f; q[1]  = 0.0f; q[2]  = 0.0f; q[3]  = column->world_position.x;
            q[4]  = 0.0f; q[5]  = 1.0f; q[6]  = 0.0f; q[7]  = column->world_position.y;
            q[8]  = 0.0f; q[9]  = 0.0f; q[10] = 1.0f; q[11] = column->world_position.z;
            q[12] = 0.0f; q[13] = 0.0f; q[14] = 0.0f; q[15] = 0.0f;
        }
    }

    callable_mp(this, &VoxelChunk::upload_buffer).call_deferred();
}

void VoxelChunk::upload_buffer() {
    RenderingServer *rs = RenderingServer::get_singleton();
    multimesh_rid = rs->multimesh_create();
    rs->multimesh_set_mesh(multimesh_rid, mesh.get_rid());
    rs->multimesh_allocate_data(multimesh_rid, columns.size(), RenderingServer::MULTIMESH_TRANSFORM_3D, false, true, false);
    rs->multimesh_set_buffer(multimesh_rid, buffer);

    instance_rid = rs->instance_create2(multimesh_rid, get_world_3d()->get_scenario());
    rs->instance_set_transform(instance_rid, get_global_transform());

    shader_rid = rs->shader_create();
    rs->shader_set_code(shader_rid, R"(
        shader_type spatial;
        render_mode unshaded;

        void fragment() {
            ALBEDO = vec3(1.0f, 0.0f, 0.0f);
        }
    )");
    material_rid = rs->material_create();
    rs->material_set_shader(material_rid, shader_rid);
    rs->instance_geometry_set_material_override(instance_rid, material_rid);

    buffer.resize(0);
}

void VoxelChunk::free_mesh() {
    RenderingServer *rs = RenderingServer::get_singleton();
    rs->free_rid(instance_rid);
    rs->free_rid(multimesh_rid);
    rs->free_rid(material_rid);
    rs->free_rid(shader_rid);
}
