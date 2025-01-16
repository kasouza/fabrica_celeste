#include "fabrica/world/chunk_map.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/world/chunk.h"

#include <stdio.h>
#include <stdlib.h>

static int cantor(int a, int b) { return (a + b + 1) * (a + b) / 2 + b; }

// Calculate a non negative index of the chunk in the buckets array, consi
// dering x, y and z can be negative, ignoring the virtual world size
static int calculate_index(int x, int y, int z, int cap) {
    return abs(cantor(cantor(x, y), z)) % cap;
}

void fabrica_chunk_map_init(fabrica_ChunkMap *chunk_map) {
    assert(chunk_map != NULL);

    chunk_map->len = 0;
    chunk_map->buckets_cap = 27;
    chunk_map->buckets =
        calloc(sizeof(fabrica_ChunkNode *), chunk_map->buckets_cap);
}

void fabrica_chunk_map_destroy(fabrica_ChunkMap *chunk_map) {
    assert(chunk_map != NULL);
    assert(chunk_map->buckets != NULL);

    for (int i = 0; i < chunk_map->buckets_cap; ++i) {
        fabrica_ChunkNode *node = chunk_map->buckets[i];
        while (node != NULL) {
            fabrica_ChunkNode *old_node = node;
            node = node->next;
            free(old_node);
        }

        chunk_map->buckets[i] = NULL;
    }

    free(chunk_map->buckets);
}

fabrica_Chunk *fabrica_chunk_map_get_by_pos(fabrica_ChunkMap *chunk_map,
                                            const fabrica_Vec3I *chunk_pos) {
    int idx = calculate_index(chunk_pos->x, chunk_pos->y, chunk_pos->z,
                              chunk_map->buckets_cap);

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    while (node != NULL) {
        if (node->pos.x == chunk_pos->x && node->pos.y == chunk_pos->y &&
            node->pos.z == chunk_pos->z) {
            return node->chunk;
        }

        node = node->next;
    }

    return NULL;
}

fabrica_Chunk *
fabrica_chunk_map_get_by_block_pos(fabrica_ChunkMap *chunk_map,
                                   const fabrica_Vec3I *block_pos) {
    fabrica_Vec3I chunk_pos;
    fabrica_block_pos_to_chunk_pos(block_pos, &chunk_pos);
    return fabrica_chunk_map_get_by_pos(chunk_map, &chunk_pos);
}

void fabrica_chunk_map_set(fabrica_ChunkMap *chunk_map, fabrica_Chunk *chunk) {
    int new_len = chunk_map->len + 1;
    float load_factor = (float)new_len / (float)chunk_map->buckets_cap;

    if (load_factor > 0.75f) {
        fprintf(stderr, "growing\n");
        fabrica_ChunkNode **old_buckets = chunk_map->buckets;
        int old_buckets_cap = chunk_map->buckets_cap;

        chunk_map->buckets_cap *= 2;
        chunk_map->buckets =
            calloc(sizeof(fabrica_ChunkNode), chunk_map->buckets_cap);
        chunk_map->len = 0;

        for (int i = 0; i < old_buckets_cap; ++i) {
            fabrica_ChunkNode *node = old_buckets[i];
            while (node != NULL) {
                fabrica_ChunkNode *next = node->next;
                fabrica_chunk_map_set(chunk_map, node->chunk);

                free(node);
                node = next;
            }
        }

        free(old_buckets);
    }

    int idx = calculate_index(chunk->pos.x, chunk->pos.y, chunk->pos.z,
                              chunk_map->buckets_cap);
    /*printf("%d: %d %d %d\n", idx, chunk->pos.x, chunk->pos.y, chunk->pos.z);*/

    fabrica_ChunkNode *new_node = malloc(sizeof(fabrica_ChunkNode));
    new_node->next = NULL;
    new_node->previous = NULL;
    new_node->chunk = chunk;
    new_node->pos = chunk->pos;

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    if (node == NULL) {
        chunk_map->buckets[idx] = new_node;
    } else {
        int i = 0;
        while (node->next != NULL) {
            assert(!(node->pos.x == new_node->pos.x &&
                     node->pos.y == new_node->pos.y &&
                     node->pos.z == new_node->pos.z));

            node = node->next;
        }

        new_node->previous = node;
        node->next = new_node;
    }

    chunk_map->len = new_len;
}

void fabrica_chunk_map_remove(fabrica_ChunkMap *chunk_map,
                              const fabrica_Vec3I *pos) {
    int idx = calculate_index(pos->x, pos->y, pos->z, chunk_map->buckets_cap);

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    while (node != NULL) {
        if (node->pos.x == pos->x && node->pos.y == pos->y &&
            node->pos.z == pos->z) {
            if (node->previous == NULL) {
                if (node->next != NULL) {
                    node->next->previous = NULL;
                }

                chunk_map->buckets[idx] = node->next;

            } else {
                if (node->previous != NULL) {
                    node->previous->next = node->next;
                }

                if (node->next != NULL) {
                    node->next->previous = node->previous;
                }
            }

            free(node);
            node = NULL;

            chunk_map->len--;

            break;
        }

        node = node->next;
    }
}

void fabrica_chunk_map_get_all(const fabrica_ChunkMap *chunk_map,
                               fabrica_Chunk ***chunks, int *len) {
    assert(chunk_map != NULL);
    assert(chunks != NULL);
    assert(len != NULL);

    *len = chunk_map->len;
    *chunks = malloc(sizeof(fabrica_Chunk) * (chunk_map->len));

    int idx = 0;
    for (int i = 0; i < chunk_map->buckets_cap; ++i) {
        fabrica_ChunkNode *node = chunk_map->buckets[i];

        while (node != NULL) {
            (*chunks)[idx] = node->chunk;
            node = node->next;
            idx += 1;
        }
    }

    /*printf("idx/len: %d %d\n", idx, *len);*/
    assert(idx == *len && "The number of chunks in the map is different from "
                          "the number of chunks returned");
}
