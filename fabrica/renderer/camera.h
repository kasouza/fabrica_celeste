#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_CAMERA_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_CAMERA_H

#include "fabrica/math/vec3f.h"
typedef struct {
    fabrica_Vec3F pos;

    fabrica_Vec3F front;
    fabrica_Vec3F up;
    fabrica_Vec3F right;

    float yaw;
    float pitch;
} fabrica_Camera;

typedef enum {
    fabrica_CameraMoveDir_FORWARD = 0,
    fabrica_CameraMoveDir_BACKWARD,
    fabrica_CameraMoveDir_LEFT,
    fabrica_CameraMoveDir_RIGHT,
    fabrica_CameraMoveDir_UP,
    fabrica_CameraMoveDir_DOWN,
    fabrica_CameraMoveDir_COUNT
} fabrica_CameraMoveDir;

void fabrica_camera_init(fabrica_Camera *camera, fabrica_Vec3F pos,
                         fabrica_Vec3F front);
void fabrica_camera_rotate(fabrica_Camera *camera, float x_offset,
                           float y_offset);
void fabrica_camera_recalculate_vectors(fabrica_Camera *camera);
void fabrica_camera_move(fabrica_Camera *camera, fabrica_CameraMoveDir dir,
                         float speed);

/**
 * @brief Calculate a 4x4 view matrix from a camera.
 *
 * @param camera The camera to calculate the view matrix from.
 * @param out_view_matrix The output view matrix. Must be a 16 float array.
 **/
void fabrica_camera_view_matrix(fabrica_Camera *camera, float *out_view_matrix);

#endif
