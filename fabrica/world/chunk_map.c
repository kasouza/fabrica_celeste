#include "fabrica/world/chunk_map.h"
#include "fabrica/renderer/chunk_mesh.h"
#include <stdio.h>
#include <stdlib.h>

int cantor(int a, int b) { return (a + b + 1) * (a + b) / 2 + b; }

// Calculate a non negative index of the chunk in the buckets array, consi
// dering x, y and z can be negative, ignoring the virtual world size
int calculate_index(int x, int y, int z, int cap) {
    return abs(cantor(cantor(x, y), z)) % cap;
}

void fabrica_chunk_map_init(fabrica_ChunkMap *chunk_map) {
    assert(chunk_map != NULL);

    chunk_map->virtual_world_size = 3;
    chunk_map->buckets_len = 0;
    chunk_map->buckets_cap = chunk_map->virtual_world_size *
                             chunk_map->virtual_world_size *
                             chunk_map->virtual_world_size;
    chunk_map->buckets =
        malloc(sizeof(fabrica_ChunkNode *) * chunk_map->buckets_cap);

    for (int i = 0; i < chunk_map->buckets_cap; ++i) {
        chunk_map->buckets[i] = NULL;
    }
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

fabrica_Chunk *fabrica_chunk_map_get(fabrica_ChunkMap *chunk_map, int x, int y,
                                     int z) {
    int idx = calculate_index(x, y, z, chunk_map->buckets_cap);

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    while (node != NULL) {
        if (node->x == x && node->y == y && node->z == z) {
            return node->chunk;
        }

        node = node->next;
    }

    return NULL;
}

void fabrica_chunk_map_set(fabrica_ChunkMap *chunk_map, int x, int y, int z,
                           fabrica_Chunk *chunk) {
    int new_len = chunk_map->buckets_len + 1;
    float load_factor = (float)new_len / (float)chunk_map->buckets_cap;

    if (load_factor > 0.75f) {
        fabrica_ChunkNode **old_buckets = chunk_map->buckets;
        int old_buckets_cap = chunk_map->buckets_cap;

        chunk_map->buckets_cap *= 2;
        chunk_map->buckets =
            malloc(sizeof(fabrica_ChunkNode) * chunk_map->buckets_cap);
        chunk_map->buckets_len = 0;

        for (int i = 0; i < chunk_map->buckets_cap; ++i) {
            chunk_map->buckets[i] = NULL;
        }

        for (int i = 0; i < old_buckets_cap; ++i) {
            fabrica_ChunkNode *node = old_buckets[i];
            while (node != NULL) {
                fabrica_ChunkNode *next = node->next;
                fabrica_chunk_map_set(chunk_map, node->x, node->y, node->z,
                                      node->chunk);
                node = next;
            }
        }

        free(old_buckets);
    }

    int idx = calculate_index(x, y, z, chunk_map->buckets_cap);

    fabrica_ChunkNode *new_node = malloc(sizeof(fabrica_ChunkNode));
    new_node->next = NULL;
    new_node->previous = NULL;
    new_node->x = x;
    new_node->y = y;
    new_node->z = z;
    new_node->chunk = chunk;

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    if (node == NULL) {
        chunk_map->buckets[idx] = new_node;
    } else {
        while (node->next != NULL) {
            node = node->next;
        }

        new_node->previous = node;
        node->next = new_node;
    }

    chunk_map->buckets_len = new_len;
}

void fabrica_chunk_map_remove(fabrica_ChunkMap *chunk_map, int x, int y,
                              int z) {
    int idx = calculate_index(x, y, z, chunk_map->buckets_cap);

    fabrica_ChunkNode *node = chunk_map->buckets[idx];

    while (node != NULL) {
        if (node->x == x && node->y == y && node->z == z) {
            if (node->previous != NULL) {
                node->previous->next = node->next;
            }

            if (node->next != NULL) {
                node->next->previous = node->previous;
            }

            free(node);
            node = NULL;

            chunk_map->buckets_len -= 1;

            break;
        }
    }
}

void fabrica_chunk_map_get_all(const fabrica_ChunkMap *chunk_map,
                               fabrica_Chunk **chunks, int *len) {
    assert(chunk_map != NULL);
    assert(chunks != NULL);
    assert(len != NULL);

    *len = chunk_map->buckets_len;
    *chunks = malloc(sizeof(fabrica_Chunk) * *len);

    int idx = 0;
    for (int i = 0; i < chunk_map->buckets_cap; ++i) {
        fabrica_ChunkNode *node = chunk_map->buckets[i];

        while (node != NULL) {
            (*chunks)[idx] = *node->chunk;
            node = node->next;
            idx += 1;
        }
    }

    assert(idx == *len && "The number of chunks in the map is different from "
                          "the number of chunks returned");
}
