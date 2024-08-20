#include <fabrica/math/mat4f.h>
#include <fabrica/math/quaternionf.h>
#include <math.h>
#include <stdio.h>

void fabrica_mat4f_identity(float *mat) {
    mat[0] = 1;
    mat[1] = 0;
    mat[2] = 0;
    mat[3] = 0;

    mat[4] = 0;
    mat[5] = 1;
    mat[6] = 0;
    mat[7] = 0;

    mat[8] = 0;
    mat[9] = 0;
    mat[10] = 1;
    mat[11] = 0;

    mat[12] = 0;
    mat[13] = 0;
    mat[14] = 0;
    mat[15] = 1;
}

void fabrica_mat4f_mult(float *a, float *b, float *out_c) {
    out_c[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
    out_c[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
    out_c[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
    out_c[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];
    out_c[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
    out_c[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
    out_c[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
    out_c[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];
    out_c[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
    out_c[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
    out_c[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
    out_c[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];
    out_c[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
    out_c[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
    out_c[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
    out_c[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];
}

void fabrica_mat4f_scaling(float x, float y, float z, float *out_c) {
    out_c[0] = x;
    out_c[1] = 0;
    out_c[2] = 0;
    out_c[3] = 0;

    out_c[4] = 0;
    out_c[5] = y;
    out_c[6] = 0;
    out_c[7] = 0;

    out_c[8] = 0;
    out_c[9] = 0;
    out_c[10] = z;
    out_c[11] = 0;

    out_c[12] = 0;
    out_c[13] = 0;
    out_c[14] = 0;
    out_c[15] = 1;
}

void fabrica_mat4f_translation(float x, float y, float z, float *out_c) {
    out_c[0] = 1;
    out_c[1] = 0;
    out_c[2] = 0;
    out_c[3] = x;

    out_c[4] = 0;
    out_c[5] = 1;
    out_c[6] = 0;
    out_c[7] = y;

    out_c[8] = 0;
    out_c[9] = 0;
    out_c[10] = 1;
    out_c[11] = z;

    out_c[12] = 0;
    out_c[13] = 0;
    out_c[14] = 0;
    out_c[15] = 1;
}

void fabrica_mat4f_rotation_from_quaternionf(fabrica_QuaternionF *quaternion,
                                              float *out) {
    float q0 = quaternion->w;
    float q1 = quaternion->x;
    float q2 = quaternion->y;
    float q3 = quaternion->z;

    // First row of the rotation matrix
    out[0] = 2 * (q0 * q0 + q1 * q1) - 1;
    out[1] = 2 * (q1 * q2 - q0 * q3);
    out[2] = 2 * (q1 * q3 + q0 * q2);
    out[3] = 0;

    // Second row of the rotation matrix
    out[4] = 2 * (q1 * q2 + q0 * q3);
    out[5] = 2 * (q0 * q0 + q2 * q2) - 1;
    out[6] = 2 * (q2 * q3 - q0 * q1);
    out[7] = 0;

    // Third row of the rotation matrix
    out[8] = 2 * (q1 * q3 - q0 * q2);
    out[9] = 2 * (q2 * q3 + q0 * q1);
    out[10] = 2 * (q0 * q0 + q3 * q3) - 1;
    out[11] = 0;

    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}

void fabrica_mat4f_persperctive(float fov, float aspect, float near, float far,
                                 float *out) {
    float tan_half_fov = tanf(fov / 2.0f);
    float f = 1.0f / tan_half_fov;
    float z_range = far - near;
    float A = (-far - near) / z_range;
    float B = (2.0f * far * near) / z_range;

    out[0] = f / aspect;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;

    out[4] = 0;
    out[5] = f;
    out[6] = 0;
    out[7] = 0;

    out[8] = 0;
    out[9] = 0;
    out[10] = A;
    out[11] = B;

    out[12] = 0;
    out[13] = 0;
    out[14] = 1;

    out[15] = 0;
}

void fabrica_mat4f_view(const fabrica_Vec3F *pos, const fabrica_Vec3F *u,
                         const fabrica_Vec3F *v, const fabrica_Vec3F *n,
                         float *out) {
    float translation[16];
    fabrica_mat4f_translation(-pos->x, -pos->y, -pos->z, translation);

    float rotation[16];
    rotation[0] = u->x;
    rotation[1] = u->y;
    rotation[2] = u->z;
    rotation[3] = 0;

    rotation[4] = v->x;
    rotation[5] = v->y;
    rotation[6] = v->z;
    rotation[7] = 0;

    rotation[8] = n->x;
    rotation[9] = n->y;
    rotation[10] = n->z;
    rotation[11] = 0;

    rotation[12] = 0;
    rotation[13] = 0;
    rotation[14] = 0;
    rotation[15] = 1;

    fabrica_mat4f_mult(rotation, translation, out);
}
