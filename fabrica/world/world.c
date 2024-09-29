#include "fabrica/world/world.h"

void fabrica_world_init(fabrica_World *world, const fabrica_TextureAtlas *atlas,
                        const fabrica_Allocator *allocator) {
    assert(world != NULL);

    world->chunks_len = WORLD_CHUNKS_LEN;

    int world_size = 3;
    for (int i = 0; i < world->chunks_len; i++) {
        fabrica_chunk_init(&world->chunks[i]);

        int x = i % world_size;
        int y = i / (world_size * world_size);
        int z = (i / world_size) % world_size;

        int x_pos = x * CHUNK_SIZE;
        int y_pos = y * CHUNK_SIZE;
        int z_pos = z * CHUNK_SIZE;

        world->chunks[i].pos = (fabrica_Vec3F){x_pos, y_pos, z_pos};

        fabrica_Chunk *chunk = &world->chunks[i];

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

        fabrica_chunk_mesh_build(&world->chunks[i], atlas);
    }
}
