#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_QUATERNIONF_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_QUATERNIONF_H

#include <fabrica/math/vec3f.h>

typedef struct {
    float w;
    float x;
    float y;
    float z;
} fabricam_QuaternionF;

void fabricam_quaternionf_normalize(fabricam_QuaternionF *q);

void fabricam_quaternionf_multiply(const fabricam_QuaternionF *a,
                                   const fabricam_QuaternionF *b,
                                   fabricam_QuaternionF *out_c);

void fabricam_quaternionf_rotation(float a, const fabricam_Vec3F *direction,
                                   fabricam_QuaternionF *out);

void fabricam_quaternionf_rotation_from_euler(float roll, float pitch,
                                              float yaw,
                                              fabricam_QuaternionF *out);

void fabricam_quaternionf_rotate(fabricam_QuaternionF *q, float a,
                                 const fabricam_Vec3F *direction);

#endif
