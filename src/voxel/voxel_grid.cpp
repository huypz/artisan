#include "voxel_grid.h"

#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "voxel/voxel_column.h"
#include "voxel/voxel_metrics.h"
#include "voxel_chunk.h"

using namespace godot;

void VoxelGrid::_bind_methods() {}

VoxelGrid::VoxelGrid() {
    chunk_count_x = 3;
    chunk_count_z = 3;
}

void VoxelGrid::_notification(int p_what) {
    switch(p_what) {
        case NOTIFICATION_EXTENSION_RELOADED:
            free();
            build();
            break;
    }
}

void VoxelGrid::_enter_tree() {
    build();
}

void VoxelGrid::_exit_tree() {
    free();
}

void VoxelGrid::create_column(int x, int z, int i) {
    Vector3 position(
        x * VoxelMetrics::LENGTH_X + 0.5f,
        0.5f,
        z * VoxelMetrics::LENGTH_Z + 0.5f
    );
    VoxelColumn* column = memnew(VoxelColumn);
    column->set_name(vformat("Column_%d_%d", x, z));
    column->set_position(position);
    // assign column to chunk
    int chunk_x = position.x / VoxelMetrics::CHUNK_SIZE_X;
    int chunk_z = position.z / VoxelMetrics::CHUNK_SIZE_Z;
    VoxelChunk *chunk = chunks[chunk_x + chunk_z * chunk_count_z];
    int local_x = x - chunk_x * VoxelMetrics::CHUNK_SIZE_X;
    int local_z = z - chunk_z * VoxelMetrics::CHUNK_SIZE_Z;
    chunk->assign_column(local_x + local_z * VoxelMetrics::CHUNK_SIZE_Z, column);
}

void VoxelGrid::create_columns() {
    int column_count_x = chunk_count_x * VoxelMetrics::CHUNK_SIZE_X;
    int column_count_z = chunk_count_z * VoxelMetrics::CHUNK_SIZE_Z;
    for (int z = 0; z < column_count_z; z++) {
        for (int x = 0; x < column_count_x; x++) {
            create_column(x, z, x + z * column_count_z);
        }
    }
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

void VoxelGrid::build() {
    create_chunks();
    create_columns();
    for (int i = 0; i < chunks.size(); i++) {
        chunks[i]->build_mesh();
    }
}

void VoxelGrid::free() {
    for (VoxelChunk* chunk : chunks) {
        if (chunk) chunk->queue_free();
    }
    chunks.clear();
}
