#include "fabrica/math/quaternionf.h"
#include <fabrica/math/vec3f.h>

#include <math.h>

void fabricam_vec3f_normalize(fabricam_Vec3F *vec) {
    float magnitude =
        sqrtf(vec->x * vec->x + vec->y * vec->y + vec->z * vec->z);

    vec->x /= magnitude;
    vec->y /= magnitude;
    vec->z /= magnitude;
}

void fabricam_vec3f_add(const fabricam_Vec3F *a, const fabricam_Vec3F *b,
                        fabricam_Vec3F *out) {
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}

void fabricam_vec3f_scale(const fabricam_Vec3F *a, float scalar,
                          fabricam_Vec3F *out) {
    out->x = a->x * scalar;
    out->y = a->y * scalar;
    out->z = a->z * scalar;
}

void fabricam_vec3f_cross_product(const fabricam_Vec3F *a,
                                  const fabricam_Vec3F *b,
                                  fabricam_Vec3F *out) {
    out->x = a->y * b->z - a->z * b->y;
    out->y = a->z * b->x - a->x * b->z;
    out->z = a->x * b->y - a->y * b->x;
}

void fabricam_vec3f_rotate(fabricam_Vec3F *vec, float angle,
                           const fabricam_Vec3F *dir) {
    fabricam_QuaternionF P = {
        0,
        vec->x,
        vec->y,
        vec->z,
    };

    float cos_half_angle = cosf(angle / 2.0);
    float sin_half_angle = sinf(angle / 2.0);

    fabricam_QuaternionF r = {cos_half_angle, sin_half_angle * dir->x,
                              sin_half_angle * dir->y, sin_half_angle * dir->z};

    fabricam_QuaternionF r_2 = {r.w, -r.x, -r.y, -r.z};

    fabricam_QuaternionF temp;
    fabricam_QuaternionF rotated;
    fabricam_quaternionf_multiply(&r, &P, &temp);
    fabricam_quaternionf_multiply(&temp, &r_2, &rotated);

    vec->x = rotated.x;
    vec->y = rotated.y;
    vec->z = rotated.z;
}
