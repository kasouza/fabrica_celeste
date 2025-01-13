#ifndef KASOUZA_FABRICA_CELESTE_FABRIC_WORLD_RAYCAST_H
#define KASOUZA_FABRICA_CELESTE_FABRIC_WORLD_RAYCAST_H

#include "fabrica/math/vec3f.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/utils/face.h"
#include "fabrica/world/world.h"

#include <stdbool.h>

#define FABRICA_RAYCAST_MAX_ITERATIONS 999999

typedef struct {
    bool hit;

    fabrica_Vec3F pos;
    fabrica_Vec3I block_pos;

    fabrica_Face face;
} fabrica_RaycastHit;

void fabrica_raycast(fabrica_World *world, const fabrica_Vec3F *pos, const fabrica_Vec3F *dir, fabrica_RaycastHit *out_hit, float max_length);

#endif
