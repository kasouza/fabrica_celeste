#include "fabrica/world/world.h"
#include "fabrica/debug.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/world/block.h"
#include "fabrica/world/chunk.h"
#include "fabrica/world/chunk_map.h"
#include <stdio.h>
#include <stdlib.h>

void fabrica_world_init(fabrica_World *world) {
    assert(world != NULL);

    fabrica_chunk_map_init(&world->chunks);
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
        fabrica_chunk_map_get_by_block_pos(&world->chunks, block_pos);
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

    return fabrica_chunk_map_get_by_block_pos(&world->chunks, block_pos);
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

void fabrica_world_unload_far_chunks(fabrica_World *world,
                                     const fabrica_Vec3F *center,
                                     int simulation_distance) {
    assert(world != NULL);
    assert(center != NULL);

    fabrica_Vec3I center_block_pos;
    fabrica_vec3i_from_vec3f(center, &center_block_pos);

    fabrica_Vec3I center_chunk_pos;
    fabrica_block_pos_to_chunk_pos(&center_block_pos, &center_chunk_pos);

    int simulation_distance_in_blocks = (simulation_distance * CHUNK_SIZE);

    int min_x = center_chunk_pos.x - simulation_distance_in_blocks;
    int min_y = center_chunk_pos.y - simulation_distance_in_blocks;
    int min_z = center_chunk_pos.z - simulation_distance_in_blocks;

    int max_x = center_chunk_pos.x + simulation_distance_in_blocks;
    int max_y = center_chunk_pos.y + simulation_distance_in_blocks;
    int max_z = center_chunk_pos.z + simulation_distance_in_blocks;

    fabrica_Chunk **chunks;
    int chunks_len;

    fabrica_chunk_map_get_all(&world->chunks, &chunks, &chunks_len);

    int toremovecount = 0;
    for (int i = 0; i < chunks_len; ++i) {
        fabrica_Chunk *chunk = chunks[i];
        assert(chunk != NULL);

        bool x_out_of_bounds = chunk->pos.x < min_x || chunk->pos.x > max_x;
        bool y_out_of_bounds = chunk->pos.y < min_y || chunk->pos.y > max_y;
        bool z_out_of_bounds = chunk->pos.z < min_z || chunk->pos.z > max_z;

        if (x_out_of_bounds || y_out_of_bounds || z_out_of_bounds) {
            toremovecount++;
            fabrica_chunk_map_remove(&world->chunks, &chunk->pos);
            free(chunk);
        }
    }

    free(chunks);
}

void fabrica_world_load_new_chunks(fabrica_World *world,
                                   const fabrica_Vec3F *center,
                                   int simulation_distance) {
    assert(world != NULL);
    assert(center != NULL);

    fabrica_Vec3I center_block_pos;
    fabrica_vec3i_from_vec3f(center, &center_block_pos);

    fabrica_Vec3I center_chunk_pos;
    fabrica_block_pos_to_chunk_pos(&center_block_pos, &center_chunk_pos);

    int simulation_distance_in_blocks = (simulation_distance * CHUNK_SIZE);

    int min_x = center_chunk_pos.x - simulation_distance_in_blocks;
    int min_y = center_chunk_pos.y - simulation_distance_in_blocks;
    int min_z = center_chunk_pos.z - simulation_distance_in_blocks;

    int max_x = center_chunk_pos.x + simulation_distance_in_blocks;
    int max_y = center_chunk_pos.y + simulation_distance_in_blocks;
    int max_z = center_chunk_pos.z + simulation_distance_in_blocks;

    for (int x = min_x; x <= max_x; x += CHUNK_SIZE) {
        for (int y = min_y; y <= max_y; y += CHUNK_SIZE) {
            for (int z = min_z; z <= max_z; z += CHUNK_SIZE) {
                fabrica_Vec3I pos = {
                    .x = x,
                    .y = y,
                    .z = z,
                };

                fabrica_Chunk *chunk =
                    fabrica_chunk_map_get_by_pos(&world->chunks, &pos);
                if (chunk != NULL) {
                    continue;
                }

                chunk = malloc(sizeof(fabrica_Chunk));
                fabrica_chunk_init(chunk);

                chunk->pos = (fabrica_Vec3I){x, y, z};

                for (int block_x = 0; block_x < CHUNK_SIZE; ++block_x) {
                    for (int block_y = 0; block_y < CHUNK_SIZE; ++block_y) {
                        for (int block_z = 0; block_z < CHUNK_SIZE; ++block_z) {
                            int idx = fabrica_chunk_block_index(
                                block_x, block_y, block_z);
                            chunk->blocks[idx].type = fabrica_BlockType_AIR;

                            if ((y + block_y) < 40) {
                                chunk->blocks[idx].type =
                                    fabrica_BlockType_STONE;
                            }

                            if ((y + block_y) >= 40 && (y + block_y) < 50) {
                                chunk->blocks[idx].type =
                                    fabrica_BlockType_DIRT;
                            }

                            if (x == 32 && y == 32 && z == 0 && block_x == 0 &&
                                block_y == 1 && block_z == 0) {
                                chunk->blocks[idx].type =
                                    fabrica_BlockType_DIRT;
                            }
                        }
                    }
                }

                fabrica_chunk_map_set(&world->chunks, chunk);
            }
        }
    }
}
