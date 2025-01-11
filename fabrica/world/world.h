#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H

#include "fabrica/world/chunk_map.h"

#define WORLD_SIZE 3

typedef struct {
    fabrica_ChunkMap chunks;
} fabrica_World;

void fabrica_world_init(fabrica_World *world, const fabrica_TextureAtlas *atlas);
void fabrica_world_destroy(fabrica_World *world);

#endif
