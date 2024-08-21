#include "fabrica/memory/malloc.h"
#include <fabrica/debug.h>
#include <fabrica/renderer/chunk_mesh.h>
#include <fabrica/world/block.h>
#include <fabrica/world/chunk.h>

void fabrica_chunk_init(fabrica_Chunk *chunk) {
    assert(chunk != NULL);

    for (int i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE); ++i) {
        chunk->blocks[i].type = fabrica_BlockType_AIR;
    }

    chunk->is_dirty = 1;

    fabrica_chunk_mesh_init(&chunk->mesh);
}
