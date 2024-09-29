#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H

#include "fabrica/world/chunk.h"

#define WORLD_CHUNKS_LEN 10

typedef struct {
    fabrica_Chunk chunks[WORLD_CHUNKS_LEN];
    int chunks_len;
} fabrica_World;

void fabrica_world_init(fabrica_World *world, const fabrica_TextureAtlas *atlas,
                        const fabrica_Allocator *allocator);
void fabrica_world_destroy(fabrica_World *world);

#endif
