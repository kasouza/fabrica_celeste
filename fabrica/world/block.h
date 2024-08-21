#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_BLOCK_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_BLOCK_H

typedef enum {
    fabrica_BlockType_AIR = 0,
    fabrica_BlockType_STONE = 1
} fabrica_BlockType;

typedef struct {
    fabrica_BlockType type;
} fabrica_Block;

#endif
