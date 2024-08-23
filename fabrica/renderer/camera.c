#include "fabrica/renderer/camera.h"

#include "fabrica/debug.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/math/vec3f.h"

#include <math.h>
#include <stddef.h>

void fabrica_camera_init(fabrica_Camera *camera, fabrica_Vec3F pos,
                         fabrica_Vec3F front) {
    assert(camera != NULL);

    fabrica_Vec3F world_up = {
        .x = 0.0f,
        .y = 1.0f,
        .z = 0.0f,
    };

    fabrica_Vec3F right;
    fabrica_vec3f_cross_product(&world_up, &front, &right);

    fabrica_Vec3F up;
    fabrica_vec3f_cross_product(&front, &right, &up);

    camera->pos = pos;
    camera->front = front;
    camera->up = up;
    camera->right = right;

    // Calculate camera angles
    fabrica_Vec3F horizontal_target = {
        .x = camera->front.x,
        .y = 0.0f,
        .z = camera->front.z,
    };
    fabrica_vec3f_normalize(&horizontal_target);

    float horizontal_angle = asinf(fabsf(horizontal_target.z));
    if (horizontal_target.z >= 0.0f) {
        if (horizontal_target.x >= 0.0f) {
            camera->yaw = (2 * M_PI) - horizontal_angle;
        } else {
            camera->yaw = M_PI + horizontal_angle;
        }

    } else {
        if (horizontal_target.x >= 0.0f) {
            camera->yaw = horizontal_angle;
        } else {
            camera->yaw = M_PI - horizontal_angle;
        }
    }

    camera->pitch = -asinf(camera->pos.y);
}

void fabrica_camera_rotate(fabrica_Camera *camera, float x_offset,
                           float y_offset) {
    camera->yaw += x_offset;
    camera->pitch += y_offset;

    if (camera->pitch > M_PI / 2.0f) {
        camera->pitch = M_PI / 2.0f;
    } else if (camera->pitch < -M_PI / 2.0f) {
        camera->pitch = -M_PI / 2.0f;
    }

    fabrica_camera_recalculate_vectors(camera);
}

void fabrica_camera_recalculate_vectors(fabrica_Camera *camera) {
    fabrica_Vec3F world_up = {0.0f, 1.0f, 0.0f};
    fabrica_Vec3F front = {1.0f, 0.0f, 0.0f};
    fabrica_vec3f_rotate(&front, camera->yaw, &world_up);
    fabrica_vec3f_normalize(&front);

    fabrica_Vec3F right;
    fabrica_vec3f_cross_product(&world_up, &front, &right);
    fabrica_vec3f_normalize(&right);

    fabrica_vec3f_rotate(&front, camera->pitch, &right);
    fabrica_vec3f_normalize(&front);

    fabrica_Vec3F up;
    fabrica_vec3f_cross_product(&front, &right, &up);

    camera->front = front;
    camera->up = up;
    camera->right = right;
}

void fabrica_camera_move(fabrica_Camera *camera, fabrica_CameraMoveDir dir,
                         float speed) {

    fabrica_Vec3F dir_vec = {0.0f, 0.0f, 0.0f};
    float velocity = speed;

    switch (dir) {
    case fabrica_CameraMoveDir_FORWARD:
        dir_vec = camera->front;
        break;

    case fabrica_CameraMoveDir_BACKWARD:
        dir_vec = camera->front;
        velocity = -velocity;
        break;

    case fabrica_CameraMoveDir_LEFT:
        dir_vec = camera->right;
        velocity = -velocity;
        break;

    case fabrica_CameraMoveDir_RIGHT:
        dir_vec = camera->right;
        break;

    case fabrica_CameraMoveDir_UP:
        dir_vec = camera->up;
        break;

    case fabrica_CameraMoveDir_DOWN:
        dir_vec = camera->up;
        velocity = -velocity;
        break;
    }

    fabrica_vec3f_normalize(&dir_vec);
    fabrica_vec3f_scale(&dir_vec, velocity, &dir_vec);
    fabrica_vec3f_add(&camera->pos, &dir_vec, &camera->pos);
}

void fabrica_camera_view_matrix(const fabrica_Camera *camera,
                                float *out_view_matrix) {
    fabrica_mat4f_view(&camera->pos, &camera->right, &camera->up,
                       &camera->front, out_view_matrix);
}
