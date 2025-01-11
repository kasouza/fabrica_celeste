#include "fabrica/math/quaternionf.h"
#include <fabrica/math/vec3i.h>

#include <math.h>
#include <stdint.h>

void fabrica_vec3i_normalize(fabrica_Vec3I *vec) {
    float magnitude =
        sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);

    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
}

void fabrica_vec3i_add(const fabrica_Vec3I *a, const fabrica_Vec3I *b,
                       fabrica_Vec3I *out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void fabrica_vec3i_scale(const fabrica_Vec3I *a, int32_t scalar,
                         fabrica_Vec3I *out) {
    out->x = a->x * scalar;
    out->y = a->y * scalar;
    out->z = a->z * scalar;
}

void fabrica_vec3i_cross_product(const fabrica_Vec3I *a, const fabrica_Vec3I *b,
                                 fabrica_Vec3I *out) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}
