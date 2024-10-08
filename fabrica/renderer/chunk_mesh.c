#include "fabrica/error.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/memory/allocator.h"
#include "fabrica/memory/malloc.h"
#include "fabrica/world/block.h"
#include <assert.h>
#include <fabrica/renderer/chunk_mesh.h>
#include <fabrica/world/chunk.h>
#include <stdio.h>
#include <stdlib.h>

#define VERTICES_INITIAL_CAPACITY 32
#define VERTICES_GROW_FACTOR 2
#define VERTICES_PER_BLOCK 36
#define VERTICES_PER_FACE 6

typedef struct {
    int down;
    int up;
    int left;
    int right;
    int front;
    int back;
} Neighbors;

void fabrica_chunk_mesh_push_block(fabrica_ChunkMesh *mesh, int x, int y, int z,
                                   const Neighbors *neighbors,
                                   const fabrica_Block *block,
                                   const fabrica_TextureAtlas *atlas);

void fabrica_chunk_mesh_init(fabrica_ChunkMesh *chunk_mesh,
                             const fabrica_Allocator *allocator) {
    assert(chunk_mesh != NULL);
    assert(allocator != NULL);
    assert(allocator->free != NULL && allocator->malloc != NULL &&
           allocator->realloc != NULL);

    chunk_mesh->vertices = NULL;
    chunk_mesh->vertices_cap = 0;
    chunk_mesh->vertices_len = 0;
    fabrica_mat4f_identity(chunk_mesh->transformation_matrix);

    chunk_mesh->allocator = allocator;
}

int isInvisible(const fabrica_Chunk *chunk, int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE) {
        return 1;
    }

    if (y < 0 || y >= CHUNK_SIZE) {
        return 1;
    }

    if (z < 0 || z >= CHUNK_SIZE) {
        return 1;
    }

    int idx = fabrica_chunk_block_index(x, y, z);
    return !fabrica_block_get_type_info(chunk->blocks[idx].type)->visible;
}

void fabrica_chunk_mesh_build(fabrica_Chunk *chunk,
                              const fabrica_TextureAtlas *atlas) {
    assert(chunk != NULL);

    fabrica_mat4f_translation(chunk->pos.x, chunk->pos.y, chunk->pos.z,
                              chunk->mesh.transformation_matrix);

    if (chunk->mesh.vertices == NULL) {
        chunk->mesh.vertices = chunk->mesh.allocator->malloc(
            sizeof(fabrica_ChunkMeshVertex) * VERTICES_INITIAL_CAPACITY);
        chunk->mesh.vertices_cap = VERTICES_INITIAL_CAPACITY;
        chunk->mesh.vertices_len = 0;
    }

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                int idx = fabrica_chunk_block_index(x, y, z);
                if (chunk->blocks[idx].type != fabrica_BlockType_AIR) {
                    Neighbors neighbors = {
                        .down = isInvisible(chunk, x, y - 1, z),
                        .up = isInvisible(chunk, x, y + 1, z),
                        .left = isInvisible(chunk, x - 1, y, z),
                        .right = isInvisible(chunk, x + 1, y, z),
                        .front = isInvisible(chunk, x, y, z - 1),
                        .back = isInvisible(chunk, x, y, z + 1)};

                    fabrica_chunk_mesh_push_block(&chunk->mesh, x, y, z,
                                                  &neighbors,
                                                  &chunk->blocks[idx], atlas);
                }
            }
        }
    }
}

void fabrica_chunk_mesh_push_block(fabrica_ChunkMesh *mesh, int x, int y, int z,
                                   const Neighbors *neighbors,
                                   const fabrica_Block *block,
                                   const fabrica_TextureAtlas *atlas) {
    assert(mesh != NULL);
    assert(mesh->vertices != NULL);

    int neighbor_count = neighbors->down + neighbors->up + neighbors->left +
                         neighbors->right + neighbors->front + neighbors->back;
    if (neighbor_count == 0) {
        return;
    }

    int vertices_count = neighbor_count * VERTICES_PER_FACE;
    int required_cap = mesh->vertices_len + vertices_count;
    if (required_cap >= mesh->vertices_cap) {
        int new_cap = mesh->vertices_cap * VERTICES_GROW_FACTOR;

        if (new_cap < required_cap) {
            new_cap = required_cap * VERTICES_GROW_FACTOR;
        }

        mesh->vertices = mesh->allocator->realloc(
            mesh->vertices, new_cap * sizeof(fabrica_ChunkMeshVertex));
        if (mesh->vertices == NULL) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }

        mesh->vertices_cap = new_cap;
    }

    const fabrica_AtlasIndices *indices =
        &fabrica_block_get_type_info(block->type)->indices;

    int idx = mesh->vertices_len;
    mesh->vertices_len += vertices_count;

    int size = atlas->size_indices;
    int resolution = atlas->image_size_pixels;

    int tex_x = (indices->first % size) * resolution;
    int tex_y = (indices->first / size) * resolution;

    int tex_x2 = tex_x + resolution;
    int tex_y2 = tex_y + resolution;

    float uv_x = (float)tex_x / atlas->size_pixels;
    float uv_y = (float)tex_y / atlas->size_pixels;

    float uv_x2 = (float)tex_x2 / atlas->size_pixels;
    float uv_y2 = (float)tex_y2 / atlas->size_pixels;


    printf("uv_x: %f, uv_y: %f, uv_x2: %f, uv_y2: %f\n", uv_x, uv_y, uv_x2, uv_y2);
    // Front face
    if (neighbors->front) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y2,
        };
    }

    // Back face
    if (neighbors->back) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };
    }

    // Left face
    if (neighbors->left) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y2,
        };
    }

    // Rigth face
    if (neighbors->right) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };
    }

    // Bottom face
    if (neighbors->down) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };
    }

    // Top face
    if (neighbors->up) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x2,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
            .u = uv_x,
            .v = uv_y,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x,
            .v = uv_y2,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
            .u = uv_x2,
            .v = uv_y2,
        };
    }
}
