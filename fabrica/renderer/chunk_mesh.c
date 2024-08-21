#include "fabrica/error.h"
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
                                   const fabrica_Allocator *allocator,
                                   const Neighbors *neighbors);

void fabrica_chunk_mesh_init(fabrica_ChunkMesh *chunk_mesh) {
    assert(chunk_mesh != NULL);

    chunk_mesh->vertices = NULL;
    chunk_mesh->vertices_cap = 0;
    chunk_mesh->vertices_len = 0;
}

int isAir(const fabrica_Chunk *chunk, int x, int y, int z) {
    if (x < 0 || x >= CHUNK_SIZE) {
        return 1;
    }

    if (y < 0 || y >= CHUNK_SIZE) {
        return 1;
    }

    if (z < 0 || z >= CHUNK_SIZE) {
        return 1;
    }

    int idx = fabrica_block_index(x, y, z);
    return chunk->blocks[idx].type == fabrica_BlockType_AIR;
}

void fabrica_mesh_chunk(fabrica_Chunk *chunk,
                        const fabrica_Allocator *allocator) {
    assert(chunk != NULL);
    assert(allocator != NULL);
    assert(allocator->free != NULL && allocator->malloc != NULL &&
           allocator->realloc != NULL);

    if (chunk->mesh.vertices == NULL) {
        chunk->mesh.vertices = allocator->malloc(
            sizeof(fabrica_ChunkMeshVertex) * VERTICES_INITIAL_CAPACITY);
        chunk->mesh.vertices_cap = VERTICES_INITIAL_CAPACITY;
        chunk->mesh.vertices_len = 0;
    }

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                int idx = fabrica_block_index(x, y, z);
                if (chunk->blocks[idx].type != fabrica_BlockType_AIR) {
                    Neighbors neighbors = {.down = isAir(chunk, x, y - 1, z),
                                           .up = isAir(chunk, x, y + 1, z),
                                           .left = isAir(chunk, x - 1, y, z),
                                           .right = isAir(chunk, x + 1, y, z),
                                           .front = isAir(chunk, x, y, z - 1),
                                           .back = isAir(chunk, x, y, z + 1)};

                    fabrica_chunk_mesh_push_block(&chunk->mesh, x, y, z,
                                                  allocator, &neighbors);
                }
            }
        }
    }
}

void fabrica_chunk_mesh_push_block(fabrica_ChunkMesh *mesh, int x, int y, int z,
                                   const fabrica_Allocator *allocator,
                                   const Neighbors *neighbors) {
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

        mesh->vertices = allocator->realloc(
            mesh->vertices, new_cap * sizeof(fabrica_ChunkMeshVertex));
        if (mesh->vertices == NULL) {
            fabrica_exit(fabrica_Error_MEMORY_ALLOCATION_ERROR);
        }

        mesh->vertices_cap = new_cap;
    }

    int idx = mesh->vertices_len;
    mesh->vertices_len += vertices_count;

    // Front face
    if (neighbors->front) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };
    }

    // Back face
    if (neighbors->back) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };
    }

    // Left face
    if (neighbors->left) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };
    }

    // Rigth face
    if (neighbors->right) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };
    }

    // Bottom face
    if (neighbors->down) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 0.0f,
            .z = z + 0.0f,
        };
    }

    // Top face
    if (neighbors->up) {
        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 0.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 0.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };

        mesh->vertices[idx++] = (fabrica_ChunkMeshVertex){
            .x = x + 1.0f,
            .y = y + 1.0f,
            .z = z + 1.0f,
        };
    }
}
