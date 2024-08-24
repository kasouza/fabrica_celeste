#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_BLOCK_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_WORLD_BLOCK_H

typedef enum {
    fabrica_BlockType_AIR = 0,
    fabrica_BlockType_STONE,
    fabrica_BlockType_DIRT,
    fabrica_BlockType_COUNT
} fabrica_BlockType;

typedef struct {
    fabrica_BlockType type;
    int visible;
    const char *texture;
} fabrica_BlockTypeInfo;

typedef struct {
    fabrica_BlockType type;
} fabrica_Block;

const fabrica_BlockTypeInfo *
fabrica_block_get_type_info(fabrica_BlockType type);

#endif
