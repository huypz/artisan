#include "voxel_metrics.h"

using namespace godot;

const Vector3 VoxelMetrics::CORNERS[] = {
    Vector3(-0.5f * LENGTH_X, 0.0f, -0.5f * LENGTH_Z),
    Vector3(0.5f * LENGTH_X, 0.0f, -0.5f * LENGTH_Z),
    Vector3(0.5f * LENGTH_X, 0.0f, 0.5f * LENGTH_Z),
    Vector3(-0.5f * LENGTH_X, 0.0f, 0.5f * LENGTH_Z)
};
