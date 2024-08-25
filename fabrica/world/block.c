#include "fabrica/world/block.h"
#include "fabrica/debug.h"
#include "fabrica/file_io/image.h"
#include "fabrica/memory/allocator.h"

#include <stddef.h>
#include <stdio.h>

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
            .texture = "assets/stone.png",
        },
        {
            .type = fabrica_BlockType_DIRT,
            .visible = 1,
            .texture = "assets/dirt.png",
        }};

void fabrica_blocks_init(const fabrica_Allocator *allocator, fabrica_TextureAtlas *atlas) {
    fabrica_BlockTypeInfo *visible_blocks[fabrica_BlockType_COUNT];
    int visible_blocks_len = 0;

    for (int i = 0; i < fabrica_BlockType_COUNT; i++) {
        visible_blocks[i] = NULL;
        if (s_fabrica_block_type_info[i].visible &&
            s_fabrica_block_type_info[i].texture) {
            int idx = visible_blocks_len++;
            visible_blocks[idx] = &s_fabrica_block_type_info[i];
        }
    }

    fabrica_Image images[fabrica_BlockType_COUNT];
    int images_len = visible_blocks_len;

    for (int i = 0; i < images_len; i++) {
        fabrica_image_load(&images[i], visible_blocks[i]->texture);
    }

    fabrica_ErrorCode error = fabrica_texture_atlas_init(atlas, 16, 4, images,
                                                         images_len, allocator);

    if (error != fabrica_ErrorCode_OK) {
        fabrica_error_print_and_clear();
        return;
    }

    for (int i = 0; i < visible_blocks_len; i++) {
        visible_blocks[i]->indices = atlas->indices[i];
    }

    for (int i = 0; i < fabrica_BlockType_COUNT; i++) {
        if (s_fabrica_block_type_info[i].visible) {
            printf("Block %d: first: %d, last: %d\n", i,
                   s_fabrica_block_type_info[i].indices.first,
                   s_fabrica_block_type_info[i].indices.last);
        }
    }
}

const fabrica_BlockTypeInfo *
fabrica_block_get_type_info(fabrica_BlockType type) {
    assert(type >= 0 && type < fabrica_BlockType_COUNT);
    return &s_fabrica_block_type_info[type];
}
