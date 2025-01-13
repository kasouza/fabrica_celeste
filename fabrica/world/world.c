#include "fabrica/world/world.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/world/block.h"
#include "fabrica/world/chunk.h"
#include "fabrica/world/chunk_map.h"
#include <stdio.h>
#include <stdlib.h>

void fabrica_world_init(fabrica_World *world,
                        const fabrica_TextureAtlas *atlas) {
    assert(world != NULL);

    fabrica_chunk_map_init(&world->chunks, WORLD_SIZE);

    for (int i = 0; i < (WORLD_SIZE * WORLD_SIZE * WORLD_SIZE); i++) {
        fabrica_Chunk *chunk = malloc(sizeof(fabrica_Chunk));
        fabrica_chunk_init(chunk);

        int x = i % WORLD_SIZE;
        int y = i / (WORLD_SIZE * WORLD_SIZE);
        int z = (i / WORLD_SIZE) % WORLD_SIZE;

        int x_pos = x * CHUNK_SIZE;
        int y_pos = y * CHUNK_SIZE;
        int z_pos = z * CHUNK_SIZE;

        chunk->pos = (fabrica_Vec3I){x_pos, y_pos, z_pos};

        for (int block_x = 0; block_x < CHUNK_SIZE; ++block_x) {
            for (int block_y = 0; block_y < CHUNK_SIZE; ++block_y) {
                for (int block_z = 0; block_z < CHUNK_SIZE; ++block_z) {
                    int idx =
                        fabrica_chunk_block_index(block_x, block_y, block_z);
                    chunk->blocks[idx].type = fabrica_BlockType_AIR;

                    if ((y_pos + block_y) < 40) {
                        chunk->blocks[idx].type = fabrica_BlockType_STONE;
                    }

                    if ((y_pos + block_y) >= 40 && (y_pos + block_y) < 50) {
                        chunk->blocks[idx].type = fabrica_BlockType_DIRT;
                    }

                    if (x_pos == 32 && y_pos == 32 && z_pos == 0 &&
                        block_x == 0 && block_y == 1 && block_z == 0) {
                        chunk->blocks[idx].type = fabrica_BlockType_DIRT;
                    }
                }
            }
        }

        fabrica_chunk_mesh_build(chunk, atlas);
        fabrica_chunk_map_set(&world->chunks, x, y, z, chunk);
    }
}

void fabrica_world_destroy(fabrica_World *world) {
    assert(world != NULL);
    fabrica_Chunk **chunks;
    int chunks_len;

    fabrica_chunk_map_get_all(&world->chunks, &chunks, &chunks_len);

    for (int i = 0; i < chunks_len; ++i) {
        fabrica_chunk_destroy(chunks[i]);
        free(chunks[i]);
    }

    free(chunks);
    fabrica_chunk_map_destroy(&world->chunks);
}

fabrica_Block *fabrica_world_get_block(fabrica_World *world,
                                       const fabrica_Vec3I *block_pos) {
    fabrica_Chunk *chunk =
        fabrica_chunk_map_get_from_block_pos(&world->chunks, block_pos);
    if (chunk == NULL) {
        return NULL;
    }

    fabrica_Vec3I pos_in_chunk = {
        .x = abs(block_pos->x - chunk->pos.x),
        .y = abs(block_pos->y - chunk->pos.y),
        .z = abs(block_pos->z - chunk->pos.z),
    };

    return fabrica_chunk_get_block(chunk, &pos_in_chunk);
}

fabrica_Chunk *
fabrica_world_get_chunk_by_block_pos(fabrica_World *world,
                                     const fabrica_Vec3I *block_pos) {
    assert(world != NULL);
    assert(block_pos != NULL);

    return fabrica_chunk_map_get_from_block_pos(&world->chunks, block_pos);
}

void fabrica_world_mark_chunk_dirty_by_block_pos(
    fabrica_World *world, const fabrica_Vec3I *block_pos) {
    assert(world != NULL);
    assert(block_pos != NULL);

    fabrica_Chunk *chunk =
        fabrica_world_get_chunk_by_block_pos(world, block_pos);

    if (chunk != NULL) {
        chunk->is_dirty = true;
    }
}
