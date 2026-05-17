#include "voxel_chunk.h"

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/classes/world3d.hpp>

#include "voxel/voxel_column.h"
#include "voxel/voxel_metrics.h"

using namespace godot;

void VoxelChunk::_bind_methods() {}

void VoxelChunk::_enter_tree() {
    columns.resize(VoxelMetrics::CHUNK_SIZE_X * VoxelMetrics::CHUNK_SIZE_Z);
    mesh.add_face(Face::RIGHT);
    mesh.add_face(Face::LEFT);
    mesh.add_face(Face::TOP);
    mesh.add_face(Face::BOTTOM);
    mesh.add_face(Face::FRONT);
    mesh.add_face(Face::BACK);
    mesh.generate_mesh();
}

void VoxelChunk::_exit_tree() {
    free_mesh();
}

void VoxelChunk::assign_column(int index, VoxelColumn* column) {
    columns[index] = column;
}

void VoxelChunk::build_buffer() {
    num_voxels = 0;
    for (int z = 0, i = 0; z < VoxelMetrics::CHUNK_SIZE_Z; z++) {
        for (int x = 0; x < VoxelMetrics::CHUNK_SIZE_X; x++) {
            VoxelColumn* column = columns[x + z * VoxelMetrics::CHUNK_SIZE_X];
            num_voxels += column->voxel_count();
        }
    }

    const uint8_t stride = 16;
    buffer.resize(num_voxels * stride);
    float *p = buffer.ptrw();
    for (int z = 0, i = 0; z < VoxelMetrics::CHUNK_SIZE_Z; z++) {
        for (int x = 0; x < VoxelMetrics::CHUNK_SIZE_X; x++) {
            VoxelColumn* column = columns[x + z * VoxelMetrics::CHUNK_SIZE_X];
            for (int y = 0; y < VoxelMetrics::CHUNK_SIZE_Y; y++) {
                Voxel &voxel = (*column)[y];
                if (voxel.type == 0) continue;
                float *q = p + (i++ * stride);
                q[0]  = 1.0f; q[1]  = 0.0f; q[2]  = 0.0f; q[3]  = column->world_x;
                q[4]  = 0.0f; q[5]  = 1.0f; q[6]  = 0.0f; q[7]  = column->world_y + y;
                q[8]  = 0.0f; q[9]  = 0.0f; q[10] = 1.0f; q[11] = column->world_z;
                q[12] = 2.0f;
                q[13] = 0.0f;
                q[14] = 0.0f;
                q[15] = 0.0f;
            }
        }
    }

    callable_mp(this, &VoxelChunk::upload_buffer).call_deferred();
}

void VoxelChunk::upload_buffer() {
    RenderingServer *rs = RenderingServer::get_singleton();
    multimesh_rid = rs->multimesh_create();
    rs->multimesh_set_mesh(multimesh_rid, mesh.get_rid());
    rs->multimesh_allocate_data(multimesh_rid, num_voxels, RenderingServer::MULTIMESH_TRANSFORM_3D, false, true, false);
    rs->multimesh_set_buffer(multimesh_rid, buffer);

    instance_rid = rs->instance_create2(multimesh_rid, get_world_3d()->get_scenario());
    rs->instance_set_transform(instance_rid, get_global_transform());

    shader_rid = rs->shader_create();
    rs->shader_set_code(shader_rid, R"(
        shader_type spatial;
        render_mode unshaded;


        global uniform sampler2D voxel_atlas : source_color, filter_nearest;
        uniform float atlas_cols = 3;
        uniform float atlas_rows = 4;

        global uniform sampler2D voxel_table : source_color, filter_nearest;
        uniform float table_cols = 6;
        uniform float table_rows = 8;

        varying vec3 normal;
        varying vec4 custom;

        void vertex() {
            normal = NORMAL;
            custom = INSTANCE_CUSTOM;
        }

        void fragment() {
            vec3 n = abs(normal);
           	float face;
           	if (n.x > 0.9)
          		face = normal.x > 0.0 ? 0.0 : 1.0;
           	else if (n.y > 0.9)
          		face = normal.y > 0.0 ? 2.0 : 3.0;
           	else if (n.z > 0.9)
          		face = normal.z > 0.0 ? 4.0 : 5.0;

           	float voxel_id = custom.r;
           	float texture_id = texture(voxel_table, vec2((face + 0.5) / table_cols, (voxel_id + 0.5) / table_rows)).r;
           	texture_id = round(texture_id);

            float col = mod(texture_id, atlas_cols);
            float row = floor(texture_id / atlas_cols);
            vec2 tile_size = vec2(
                1.0f / float(atlas_cols),
                1.0f / float(atlas_rows)
            );
            vec2 tile_offset = vec2(col, row) * tile_size;
            vec2 final_uv = tile_offset + UV * tile_size;

            ALBEDO = texture(voxel_atlas, final_uv).rgb;
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

VoxelColumn* VoxelChunk::get_column(int local_x, int local_z) {
    if (local_x < 0 || local_z < 0) {
        return nullptr;
    }

    int index = local_x + local_z * VoxelMetrics::CHUNK_SIZE_X;
    return index < columns.size() ? columns[index] : nullptr;
}
