#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3F_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MATH_VEC3F_H

typedef struct {
    float x;
    float y;
    float z;
} fabrica_Vec3F;

void fabrica_vec3f_normalize(fabrica_Vec3F *vec);
void fabrica_vec3f_add(const fabrica_Vec3F *a, const fabrica_Vec3F *b,
                        fabrica_Vec3F *out);
void fabrica_vec3f_scale(const fabrica_Vec3F *a, float scalar,
                          fabrica_Vec3F *out);
void fabrica_vec3f_cross_product(const fabrica_Vec3F *a,
                                  const fabrica_Vec3F *b, fabrica_Vec3F *out);
void fabrica_vec3f_rotate(fabrica_Vec3F *vec, float angle, const fabrica_Vec3F *dir);

#endif
