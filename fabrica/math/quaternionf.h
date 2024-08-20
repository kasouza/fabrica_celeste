#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_QUATERNIONF_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_QUATERNIONF_H

#include <fabrica/math/vec3f.h>

typedef struct {
    float w;
    float x;
    float y;
    float z;
} fabrica_QuaternionF;

void fabrica_quaternionf_normalize(fabrica_QuaternionF *q);

void fabrica_quaternionf_multiply(const fabrica_QuaternionF *a,
                                   const fabrica_QuaternionF *b,
                                   fabrica_QuaternionF *out_c);

void fabrica_quaternionf_rotation(float a, const fabrica_Vec3F *direction,
                                   fabrica_QuaternionF *out);

void fabrica_quaternionf_rotation_from_euler(float roll, float pitch,
                                              float yaw,
                                              fabrica_QuaternionF *out);

void fabrica_quaternionf_rotate(fabrica_QuaternionF *q, float a,
                                 const fabrica_Vec3F *direction);

#endif
