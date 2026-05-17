#include "voxel_mesh.h"

#include "voxel.h"
#include "voxel_metrics.h"

using namespace godot;

void VoxelMesh::build(Voxel* voxels) {
    vertices.clear();
    normals.clear();
    uvs.clear();

    for (int z = 0; z < VoxelMetrics::CHUNK_SIZE_Z; z++) {
        for (int y = 0; y < VoxelMetrics::CHUNK_SIZE_Y; y++) {
            for (int x = 0; x < VoxelMetrics::CHUNK_SIZE_X; x++) {
                int i = x + y * VoxelMetrics::CHUNK_SIZE_X + z * VoxelMetrics::CHUNK_SIZE_X * VoxelMetrics::CHUNK_SIZE_Y;
                Voxel* voxel = &voxels[i];
                if (voxel->type == 0) {
                    continue;
                }
                Vector3 origin(x + 0.5f, y + 0.5f, z + 0.5f);
                add_face(Face::RIGHT, voxel, origin);
                add_face(Face::LEFT, voxel, origin);
                add_face(Face::TOP, voxel, origin);
                add_face(Face::BOTTOM, voxel, origin);
                add_face(Face::FRONT, voxel, origin);
                add_face(Face::BACK, voxel, origin);
            }
        }
    }
}

Ref<ArrayMesh> VoxelMesh::get_mesh() {
    ERR_FAIL_COND_V_MSG(vertices.is_empty(), Ref<ArrayMesh>(), "VoxelMeshBuilder::get_mesh() - vertices array is empty, call build() first");

    Array surface;
    surface.resize(Mesh::ARRAY_MAX);
    surface[Mesh::ARRAY_VERTEX] = vertices;
    surface[Mesh::ARRAY_NORMAL] = normals;
    surface[Mesh::ARRAY_TEX_UV] = uvs;

    Ref<ArrayMesh> mesh;
    mesh.instantiate();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface);
    return mesh;
}

void VoxelMesh::add_face(Face face, Voxel* voxel, Vector3 origin) {
    ERR_FAIL_COND_MSG(voxel == nullptr, "VoxelMesh::add_face() - voxel pointer is null, initialization error");

    Vector2i atlas_index(
        voxel->face_tex[face] % VoxelMetrics::ATLAS_COLS,
        voxel->face_tex[face] / VoxelMetrics::ATLAS_COLS
    );
    Vector2 uv_offset(
        atlas_index.x * VoxelMetrics::TILE_SIZE_U,
        atlas_index.y * VoxelMetrics::TILE_SIZE_V
    );

    for (int i = 0; i < 6; i++) {
        int vi = face_indices[face * 6 + i];
        vertices.push_back(origin + cube_vertices[vi]);
        normals.push_back(face_normals[face]);

        Vector2 uv = uv_offset + Vector2(
            face_uvs[i].x * VoxelMetrics::TILE_SIZE_U,
            face_uvs[i].y * VoxelMetrics::TILE_SIZE_V
        );
        uvs.push_back(uv);
    }
}
