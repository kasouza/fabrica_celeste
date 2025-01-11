#ifndef KASOUZA_FABRICA_CELESTE_FABRIC_WORLD_RAYCAST_H
#define KASOUZA_FABRICA_CELESTE_FABRIC_WORLD_RAYCAST_H

#include "fabrica/math/vec3f.h"
#include <stdbool.h>

typedef struct {
    bool hit;
    fabrica_Vec3F pos;
} fabrica_RaycastHit;

#endif
