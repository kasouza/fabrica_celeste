#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H

#include "fabrica/world/chunk.h"


typedef struct {
    fabrica_Chunk chunks[1];
    int chunks_len;
} fabrica_World;

void fabrica_world_init(fabrica_World *world, const fabrica_Allocator *allocator);
void fabrica_world_destroy(fabrica_World *world);

#endif
