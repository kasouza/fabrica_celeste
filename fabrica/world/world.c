#include "fabrica/world/world.h"

void fabrica_world_init(fabrica_World *world,
                        const fabrica_Allocator *allocator) {
    assert(world != NULL);

    world->chunks_len = 1;
    fabrica_chunk_init(&world->chunks[0], allocator);
    fabrica_chunk_mesh_build(&world->chunks[0]);
}
