#include "fabrica/world/block.h"
#include "fabrica/debug.h"

#include <stddef.h>

static fabrica_BlockTypeInfo
    s_fabrica_block_type_info[fabrica_BlockType_COUNT] = {
        {
            .type = fabrica_BlockType_AIR,
            .visible = 0,
            .texture = NULL,
        },
        {
            .type = fabrica_BlockType_STONE,
            .visible = 1,
            .texture = "stone.png",
        },
        {
            .type = fabrica_BlockType_DIRT,
            .visible = 1,
            .texture = "dirt.png",
        }};

const fabrica_BlockTypeInfo *
fabrica_block_get_type_info(fabrica_BlockType type) {
    assert(type >= 0 && type < fabrica_BlockType_COUNT);
    return &s_fabrica_block_type_info[type];
}
