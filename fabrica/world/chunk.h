#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_H

#include "fabrica/debug.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/world/block.h"
#include <stdbool.h>

#define CHUNK_SIZE 32

typedef struct fabrick_Chunk {
    fabrica_Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    fabrica_ChunkMesh mesh;

    fabrica_Vec3I pos;
    bool is_dirty;
} fabrica_Chunk;

/**
 * Calculates the index of a block in a chunk's block buffer
 * x, y and z represent the coordinates of a block in the chunk
 */
static inline int fabrica_chunk_block_index(int x, int y, int z) {
    assert(x >= 0 && x < CHUNK_SIZE);
    assert(y >= 0 && y < CHUNK_SIZE);
    assert(z >= 0 && z < CHUNK_SIZE);

    return (x * CHUNK_SIZE * CHUNK_SIZE) + (y * CHUNK_SIZE) + z;
}

void fabrica_chunk_init(fabrica_Chunk *chunk);
void fabrica_chunk_destroy(fabrica_Chunk *chunk);

void fabrica_block_pos_to_chunk_pos(const fabrica_Vec3I *block_pos,
                                    fabrica_Vec3I *out_chunk_pos);

fabrica_Block *fabrica_chunk_get_block(fabrica_Chunk *chunk, const fabrica_Vec3I *pos_in_chunk);

#endif
