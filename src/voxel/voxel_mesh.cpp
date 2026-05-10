#include "voxel_mesh.h"

using namespace godot;

void VoxelMesh::add_face(Face face) {
    for (int i = 0; i < 6; i++) {
        int vi = face_indices[face * 6 + i];
        vertices.push_back(cube_vertices[vi]);
        normals.push_back(face_normals[face]);
        uvs.push_back(face_uvs[i]);
    }
}

void VoxelMesh::generate_mesh() {
    if (mesh.is_valid()) return;

    surface_array.resize(Mesh::ARRAY_MAX);
    surface_array[Mesh::ARRAY_VERTEX] = vertices;
    surface_array[Mesh::ARRAY_NORMAL] = normals;
    surface_array[Mesh::ARRAY_TEX_UV] = uvs;

    mesh.instantiate();
    mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, surface_array);
}

RID VoxelMesh::get_rid() const { return mesh->get_rid(); }
