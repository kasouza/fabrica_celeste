#include "fabrica/world/world.h"
#include "fabrica/world/chunk_map.h"
#include <stdlib.h>

void fabrica_world_init(fabrica_World *world, const fabrica_TextureAtlas *atlas) {
    assert(world != NULL);

    fabrica_chunk_map_init(&world->chunks);

    for (int i = 0; i < (WORLD_SIZE * WORLD_SIZE * WORLD_SIZE); i++) {
        fabrica_Chunk *chunk = malloc(sizeof(fabrica_Chunk));
        fabrica_chunk_init(chunk);

        int x = i % WORLD_SIZE;
        int y = i / (WORLD_SIZE * WORLD_SIZE);
        int z = (i / WORLD_SIZE) % WORLD_SIZE;

        int x_pos = x * CHUNK_SIZE;
        int y_pos = y * CHUNK_SIZE;
        int z_pos = z * CHUNK_SIZE;

        chunk->pos = (fabrica_Vec3F){x_pos, y_pos, z_pos};

        for (int block_x = 0; block_x < CHUNK_SIZE; ++block_x) {
            for (int block_y = 0; block_y < CHUNK_SIZE; ++block_y) {
                for (int block_z = 0; block_z < CHUNK_SIZE; ++block_z) {
                    int idx = fabrica_chunk_block_index(block_x, block_y, block_z);

                    chunk->blocks[idx].type = fabrica_BlockType_AIR;

                    if ((y_pos + block_y) < 40) {
                        chunk->blocks[idx].type = fabrica_BlockType_STONE;
                    }

                    if ((y_pos + block_y) >= 40 && (y_pos + block_y) < 50) {
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
    fabrica_chunk_map_destroy(&world->chunks);
}
