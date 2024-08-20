#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_MAT4_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_MAT4_H

#include <fabrica/math/quaternionf.h>

void fabricam_mat4f_identity(float *mat);

void fabricam_mat4f_mult(float *a, float *b, float *out_c);

void fabricam_mat4f_scaling(float x, float y, float z, float *out_c);
void fabricam_mat4f_translation(float x, float y, float z, float *out_c);

void fabricam_mat4f_rotation_from_quaternionf(fabricam_QuaternionF *quaternion,
                                              float *out);
void fabricam_mat4f_persperctive(float fov, float aspect, float near, float far,
                                float *out);

void fabricam_mat4f_view(const fabricam_Vec3F *pos, const fabricam_Vec3F *u,
                        const fabricam_Vec3F *v, const fabricam_Vec3F *n,
                        float *out);

#endif
