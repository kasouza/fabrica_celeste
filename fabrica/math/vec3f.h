#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3F_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3F_H

typedef struct {
    float x;
    float y;
    float z;
} fabricam_Vec3F;

void fabricam_vec3f_normalize(fabricam_Vec3F *vec);
void fabricam_vec3f_add(const fabricam_Vec3F *a, const fabricam_Vec3F *b,
                        fabricam_Vec3F *out);
void fabricam_vec3f_scale(const fabricam_Vec3F *a, float scalar,
                          fabricam_Vec3F *out);
void fabricam_vec3f_cross_product(const fabricam_Vec3F *a,
                                  const fabricam_Vec3F *b, fabricam_Vec3F *out);
void fabricam_vec3f_rotate(fabricam_Vec3F *vec, float angle, const fabricam_Vec3F *dir);

#endif
