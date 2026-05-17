#include "voxel_column.h"

#include "voxel/voxel_metrics.h"

void VoxelColumn::_bind_methods() {}

VoxelColumn::VoxelColumn() {
    voxels.resize(VoxelMetrics::CHUNK_SIZE_Y);

    for (int i = 0; i < voxels.size(); i++) {
        voxels[i].type = 0;
    }
    voxels[0].type = 1;
}

Voxel& VoxelColumn::operator[](int y) {
    return voxels[y];
}

int VoxelColumn::voxel_count() const {
    int count = 0;
    for (int y = 0; y < VoxelMetrics::CHUNK_SIZE_Y; y++) {
        if (voxels[y].type != 0) count++;
    }
    return count;
}
