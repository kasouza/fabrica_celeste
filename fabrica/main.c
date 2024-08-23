#include "fabrica/error.h"
#include "fabrica/file_io/read_file.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/math/quaternionf.h"
#include "fabrica/math/vec3f.h"
#include "fabrica/memory/allocator.h"
#include "fabrica/renderer/camera.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/renderer/gl.h"
#include "fabrica/renderer/renderer.h"
#include "fabrica/renderer/shaders.h"
#include "fabrica/world/block.h"
#include "fabrica/world/chunk.h"
#include "fabrica/world/world.h"

#include <GLFW/glfw3.h>
#include <alloca.h>
#include <stb/stb_image.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TO_RADIAN(x) (float)(((x)*M_PI / 180.0f))

static int s_is_running = 1;

static double s_cursor_pos_x;
static double s_cursor_pos_y;

static fabrica_Camera s_camera;

void handle_key_event(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        s_is_running = 0;
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        fabrica_CameraMoveDir dir;
        switch (key) {
        case GLFW_KEY_W:
            dir = fabrica_CameraMoveDir_FORWARD;
            break;

        case GLFW_KEY_S:
            dir = fabrica_CameraMoveDir_BACKWARD;
            break;

        case GLFW_KEY_D:
            dir = fabrica_CameraMoveDir_RIGHT;
            break;

        case GLFW_KEY_A:
            dir = fabrica_CameraMoveDir_LEFT;
            break;

        case GLFW_KEY_SPACE:
            dir = fabrica_CameraMoveDir_UP;
            break;

        case GLFW_KEY_LEFT_SHIFT:
            dir = fabrica_CameraMoveDir_DOWN;
            break;
        }

        fabrica_camera_move(&s_camera, dir, 0.1);
    }
}

void handle_window_close_event(GLFWwindow *window) { s_is_running = 0; }

void handle_cursor_pos_event(GLFWwindow *window, double x, double y) {
    float x_offset = (x - s_cursor_pos_x) * 0.01;
    float y_offset = (y - s_cursor_pos_y) * 0.01;

    s_cursor_pos_x = x;
    s_cursor_pos_y = y;

    fabrica_camera_rotate(&s_camera, x_offset, y_offset);
    fabrica_camera_recalculate_vectors(&s_camera);
}

void init_events(GLFWwindow *window) {
    glfwSetKeyCallback(window, handle_key_event);
    glfwSetWindowCloseCallback(window, handle_window_close_event);
    glfwSetCursorPosCallback(window, handle_cursor_pos_event);
}

int main() {
    fabrica_Allocator default_allocator = {
        .malloc = malloc, .free = free, .realloc = realloc};

    fabrica_error_init(&default_allocator);
    fabrica_renderer_init(&default_allocator);
    init_events(fabrica_renderer_get_window());

    fabrica_camera_init(&s_camera, (fabrica_Vec3F){0.0f, 0.0f, -1.0f},
                        (fabrica_Vec3F){0.0f, 0.0f, 1.0f});

    glfwGetCursorPos(fabrica_renderer_get_window(), &s_cursor_pos_x,
                     &s_cursor_pos_y);

    fabrica_World world;
    world.chunks_len = 1;
    fabrica_chunk_init(&world.chunks[0], &default_allocator);
    fabrica_chunk_mesh_build(&world.chunks[0]);

    while (s_is_running) {
        fabrica_render(&world, &s_camera);
    }

    fabrica_error_terminate();
    glfwTerminate();
}
