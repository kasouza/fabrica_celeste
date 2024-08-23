#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_WORLD_H

#include "fabrica/world/chunk.h"


typedef struct {
    fabrica_Chunk chunks[1];
    int chunks_len;
} fabrica_World;

#endif
