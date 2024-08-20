#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_MAT4_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_MAT4_H

#include <fabrica/math/quaternionf.h>

void fabrica_mat4f_identity(float *mat);

void fabrica_mat4f_mult(float *a, float *b, float *out_c);

void fabrica_mat4f_scaling(float x, float y, float z, float *out_c);
void fabrica_mat4f_translation(float x, float y, float z, float *out_c);

void fabrica_mat4f_rotation_from_quaternionf(fabrica_QuaternionF *quaternion,
                                              float *out);
void fabrica_mat4f_persperctive(float fov, float aspect, float near, float far,
                                float *out);

void fabrica_mat4f_view(const fabrica_Vec3F *pos, const fabrica_Vec3F *u,
                        const fabrica_Vec3F *v, const fabrica_Vec3F *n,
                        float *out);

#endif
