#include "fabrica/memory/allocator.h"
#include "fabrica/memory/malloc.h"
#include <fabrica/debug.h>
#include <fabrica/renderer/chunk_mesh.h>
#include <fabrica/world/block.h>
#include <fabrica/world/chunk.h>

void fabrica_chunk_init(fabrica_Chunk *chunk,
                        const fabrica_Allocator *allocator) {
    assert(chunk != NULL);

    for (int i = 0; i < (CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE); ++i) {
        chunk->blocks[i].type = fabrica_BlockType_STONE;
    }

    chunk->is_dirty = 1;
    chunk->pos.x = 0;
    chunk->pos.y = 0;
    chunk->pos.z = 0;

    fabrica_chunk_mesh_init(&chunk->mesh, allocator);
}
