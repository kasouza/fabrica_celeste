#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_MAP_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_CHUNK_MAP_H

#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/world/chunk.h"

typedef struct fabrica_ChunkNode {
    fabrica_Vec3I pos;
    fabrica_Chunk *chunk;
    struct fabrica_ChunkNode *next;
    struct fabrica_ChunkNode *previous;
} fabrica_ChunkNode;

typedef struct {
    fabrica_ChunkNode **buckets;
    int len;
    int buckets_cap;
} fabrica_ChunkMap;

void fabrica_chunk_map_init(fabrica_ChunkMap *chunk_map);
void fabrica_chunk_map_destroy(fabrica_ChunkMap *chunk_map);

fabrica_Chunk *fabrica_chunk_map_get_by_pos(fabrica_ChunkMap *chunk_map,
                                            const fabrica_Vec3I *chunk_pos);
fabrica_Chunk *
fabrica_chunk_map_get_by_block_pos(fabrica_ChunkMap *chunk_map,
                                   const fabrica_Vec3I *block_pos);
void fabrica_chunk_map_set(fabrica_ChunkMap *chunk_map, fabrica_Chunk *chunk);
void fabrica_chunk_map_remove(fabrica_ChunkMap *chunk_map,
                              const fabrica_Vec3I *pos);

/**
 * Get all chunks in the chunk map
 * @param chunk_map The chunk map
 * @param out_chunks The output array of chunks, will be allocated by the
 * function, must be freed by the caller
 * @param out_len The output length of the array
 */
void fabrica_chunk_map_get_all(const fabrica_ChunkMap *chunk_map,
                               fabrica_Chunk ***out_chunks, int *out_len);

#endif
