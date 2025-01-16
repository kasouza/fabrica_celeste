#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3I_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3I_H

#include "fabrica/math/vec3f.h"
#include <stdint.h>
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} fabrica_Vec3I;

void fabrica_vec3i_normalize(fabrica_Vec3I *vec);
void fabrica_vec3i_add(const fabrica_Vec3I *a, const fabrica_Vec3I *b,
                       fabrica_Vec3I *out);
void fabrica_vec3i_scale(const fabrica_Vec3I *a, int32_t scalar,
                         fabrica_Vec3I *out);

void fabrica_vec3i_from_vec3f(const fabrica_Vec3F *v, fabrica_Vec3I *out);

#endif
