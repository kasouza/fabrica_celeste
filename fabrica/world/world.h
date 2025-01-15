#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H

#include "fabrica/math/vec3i.h"
#include "fabrica/world/chunk_map.h"

#define WORLD_SIZE 3

typedef struct {
    fabrica_ChunkMap chunks;
} fabrica_World;

void fabrica_world_init(fabrica_World *world);
void fabrica_world_destroy(fabrica_World *world);

fabrica_Block *fabrica_world_get_block(fabrica_World *world, const fabrica_Vec3I *block_pos);
fabrica_Chunk *fabrica_world_get_chunk_by_block_pos(fabrica_World *world, const fabrica_Vec3I *block_pos);

void fabrica_world_mark_chunk_dirty_by_block_pos(fabrica_World *world, const fabrica_Vec3I *block_pos);

#endif
