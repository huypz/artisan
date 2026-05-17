#pragma once

class VoxelMetrics {
public:
    // Size of one voxel in world units
    static constexpr float VOXEL_SIZE_X = 1.0f,
                           VOXEL_SIZE_Y = 1.0f,
                           VOXEL_SIZE_Z = 1.0f;
    // Chunk dimensions in voxels
    static constexpr int CHUNK_SIZE_X = 8,
                         CHUNK_SIZE_Y = 8,
                         CHUNK_SIZE_Z = 8;
    // Atlas and texture UVs
    static constexpr int ATLAS_COLS = 3;
    static constexpr int ATLAS_ROWS = 4;
    static constexpr float TILE_WIDTH = 8.0f;
    static constexpr float TILE_HEIGHT = 8.0f;
    static constexpr float TILE_SIZE_U = 1.0f / ATLAS_COLS;
    static constexpr float TILE_SIZE_V = 1.0f / ATLAS_ROWS;
};
