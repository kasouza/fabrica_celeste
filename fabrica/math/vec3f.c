#include "fabrica/math/quaternionf.h"
#include <fabrica/math/vec3f.h>

#include <math.h>

void fabrica_vec3f_normalize(fabrica_Vec3F *vec) {
    float magnitude =
        sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);

    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
}

void fabrica_vec3f_add(const fabrica_Vec3F *a, const fabrica_Vec3F *b,
                        fabrica_Vec3F *out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void fabrica_vec3f_scale(const fabrica_Vec3F *a, float scalar,
                          fabrica_Vec3F *out) {
    out->x = a->x * scalar;
    out->y = a->y * scalar;
    out->z = a->z * scalar;
}

void fabrica_vec3f_cross_product(const fabrica_Vec3F *a,
                                  const fabrica_Vec3F *b,
                                  fabrica_Vec3F *out) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

void fabrica_vec3f_rotate(fabrica_Vec3F *vec, float angle,
                           const fabrica_Vec3F *dir) {
    fabrica_QuaternionF P = {
        0,
        vec->x,
        vec->y,
        vec->z,
    };

    float cos_half_angle = cosf(angle / 2.0);
    float sin_half_angle = sinf(angle / 2.0);

    fabrica_QuaternionF r = {cos_half_angle, sin_half_angle * dir->x,
                              sin_half_angle * dir->y, sin_half_angle * dir->z};

    fabrica_QuaternionF r_2 = {r.w, -r.x, -r.y, -r.z};

    fabrica_QuaternionF temp;
    fabrica_QuaternionF rotated;
    fabrica_quaternionf_multiply(&r, &P, &temp);
    fabrica_quaternionf_multiply(&temp, &r_2, &rotated);

    vec->x = rotated.x;
    vec->y = rotated.y;
    vec->z = rotated.z;
}
