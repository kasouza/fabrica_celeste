#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_H

#include "fabrica/memory/malloc.h"
#include <fabrica/renderer/chunk_mesh.h>
#include <fabrica/world/block.h>

#define CHUNK_SIZE 32

typedef struct fabrick_Chunk {
    fabrica_Block blocks[CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE];
    fabrica_ChunkMesh mesh;
    int is_dirty;
} fabrica_Chunk;

static inline int fabrica_block_index(int x, int y, int z) {
    return (x * CHUNK_SIZE * CHUNK_SIZE) + (y * CHUNK_SIZE) + z;
}

void fabrica_chunk_init(fabrica_Chunk *chunk);

#endif
