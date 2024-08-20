#include "fabrica/math/vec3f.h"
#include <fabrica/math/quaternionf.h>
#include <math.h>

void fabricam_quaternionf_normalize(fabricam_QuaternionF *q) {
    float w = q->w;
    float x = q->x;
    float y = q->y;
    float z = q->z;

    float magnitude = sqrtf(w * w + x * x + y * y + z * z);
    q->w = w / magnitude;
    q->x = x / magnitude;
    q->y = y / magnitude;
    q->z = z / magnitude;
}

void fabricam_quaternionf_multiply(const fabricam_QuaternionF *a,
                                   const fabricam_QuaternionF *b,
                                   fabricam_QuaternionF *out_c) {
    float w = a->w * b->w - a->x * b->x - a->y * b->y - a->z * b->z;
    float x = a->w * b->x + a->x * b->w + a->y * b->z - a->z * b->y;
    float y = a->w * b->y - a->x * b->z + a->y * b->w + a->z * b->x;
    float z = a->w * b->z + a->x * b->y - a->y * b->x + a->z * b->w;

    out_c->w = w;
    out_c->x = x;
    out_c->y = y;
    out_c->z = z;
}

void fabricam_quaternionf_rotation(float a, const fabricam_Vec3F *direction,
                                   fabricam_QuaternionF *out) {
    float cos_half_alpha = cosf(a / 2.0);
    float sin_half_alpha = sinf(a / 2.0);

    out->w = cos_half_alpha;
    out->x = sin_half_alpha * direction->x;
    out->y = sin_half_alpha * direction->y;
    out->z = sin_half_alpha * direction->z;
}

void fabricam_quaternionf_rotation_from_euler(float roll, float pitch,
                                              float yaw,
                                              fabricam_QuaternionF *out) {
    float cr = cosf(roll * 0.5);
    float sr = sinf(roll * 0.5);
    float cp = cosf(pitch * 0.5);
    float sp = sinf(pitch * 0.5);
    float cy = cosf(yaw * 0.5);
    float sy = sinf(yaw * 0.5);

    out->w = cr * cp * cy + sr * sp * sy;
    out->x = sr * cp * cy - cr * sp * sy;
    out->y = cr * sp * cy + sr * cp * sy;
    out->z = cr * cp * sy - sr * sp * cy;
}

void fabricam_quaternionf_rotate(fabricam_QuaternionF *q, float a,
                               const fabricam_Vec3F *direction) {
    fabricam_QuaternionF rotation;
    fabricam_quaternionf_rotation(a, direction, &rotation);
    fabricam_quaternionf_multiply(q, &rotation, q);
}
