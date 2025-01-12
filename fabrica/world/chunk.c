#include "fabrica/debug.h"
#include <fabrica/renderer/chunk_mesh.h>
#include <fabrica/world/block.h>
#include <fabrica/world/chunk.h>
#include <math.h>
#include <stdio.h>
#include <time.h>

void fabrica_chunk_init(fabrica_Chunk *chunk) {
    assert(chunk != NULL);

    for (int i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE); ++i) {
        chunk->blocks[i].type = fabrica_BlockType_STONE;
    }

    chunk->is_dirty = true;
    chunk->pos.x = 0;
    chunk->pos.y = 0;
    chunk->pos.z = 0;

    fabrica_chunk_mesh_init(&chunk->mesh);
}

void fabrica_chunk_destroy(fabrica_Chunk *chunk) {
    assert(chunk != NULL);
    fabrica_chunk_mesh_destroy(&chunk->mesh);
}

void fabrica_block_pos_to_chunk_pos(const fabrica_Vec3I *block_pos,
                                    fabrica_Vec3I *out_chunk_pos) {
    assert(out_chunk_pos != NULL);
    out_chunk_pos->x = floorf((double)block_pos->x / CHUNK_SIZE);
    out_chunk_pos->y = floorf((double)block_pos->y / CHUNK_SIZE);
    out_chunk_pos->z = floorf((double)block_pos->z / CHUNK_SIZE);
}

fabrica_Block *fabrica_chunk_get_block(fabrica_Chunk *chunk, const fabrica_Vec3I *pos_in_chunk) {
    assert(pos_in_chunk != NULL);

    if (pos_in_chunk->x < 0 || pos_in_chunk->x >= CHUNK_SIZE) {
        return NULL;
    }

    if (pos_in_chunk->y < 0 || pos_in_chunk->y >= CHUNK_SIZE) {
        return NULL;
    }

    if (pos_in_chunk->z < 0 || pos_in_chunk->z >= CHUNK_SIZE) {
        return NULL;
    }

    int idx = fabrica_chunk_block_index(pos_in_chunk->x, pos_in_chunk->y, pos_in_chunk->z);
    return &chunk->blocks[idx];
}
