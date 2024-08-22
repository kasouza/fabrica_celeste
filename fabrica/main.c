#include "fabrica/error.h"
#include "fabrica/file_io/read_file.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/math/quaternionf.h"
#include "fabrica/math/vec3f.h"
#include "fabrica/memory/allocator.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/renderer/chunk_renderer.h"
#include "fabrica/renderer/gl.h"
#include "fabrica/renderer/shaders.h"
#include "fabrica/world/block.h"
#include "fabrica/world/chunk.h"

#include <stb/stb_image.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TO_RADIAN(x) (float)(((x)*M_PI / 180.0f))

static GLFWwindow *s_window = NULL;
static int s_window_width = 800;
static int s_window_height = 600;

static int s_is_running = 1;

static fabrica_Vec3F s_camera_pos = {0};

static float s_camera_yaw = M_PI / 2;
static float s_camera_pitch = 0.0f;

static double s_cursor_pos_x;
static double s_cursor_pos_y;

static fabrica_Vec3F s_camera_right = {
    1.0f,
    0.0f,
    0.0f,
};

static fabrica_Vec3F s_camera_up = {
    0.0f,
    1.0f,
    0.0f,
};

static fabrica_Vec3F s_camera_front = {
    0.0f,
    0.0f,
    1.0f,
};

void init_gl() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    s_window =
        glfwCreateWindow(s_window_width, s_window_height, "Mine", NULL, NULL);
    if (!s_window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(s_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialize GLAD\n");
        glfwTerminate();
        return;
    }

    glViewport(0, 0, s_window_width, s_window_height);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
}

void camera_recalculate_vectors() {
    fabrica_Vec3F world_up = {0.0f, 1.0f, 0.0f};
    fabrica_Vec3F front = {1.0f, 0.0f, 0.0f};
    fabrica_vec3f_rotate(&front, s_camera_yaw, &world_up);
    fabrica_vec3f_normalize(&front);

    fabrica_Vec3F right;
    fabrica_vec3f_cross_product(&world_up, &front, &right);
    fabrica_vec3f_normalize(&right);

    fabrica_vec3f_rotate(&front, s_camera_pitch, &right);
    fabrica_vec3f_normalize(&front);

    fabrica_Vec3F up;
    fabrica_vec3f_cross_product(&front, &right, &up);

    s_camera_front = front;
    s_camera_up = up;
    s_camera_right = right;
}

void handle_window_resize_event(GLFWwindow *window, int width, int height) {
    s_window_width = width;
    s_window_height = height;

    glViewport(0, 0, width, height);
}

void handle_key_event(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        s_is_running = 0;
    }

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabrica_Vec3F velocity;

        fabrica_vec3f_scale(&s_camera_front, 0.1, &velocity);
        fabrica_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    } else if (key == GLFW_KEY_S &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabrica_Vec3F velocity;

        fabrica_vec3f_scale(&s_camera_front, -0.1, &velocity);
        fabrica_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabrica_Vec3F velocity;

        fabrica_vec3f_scale(&s_camera_right, 0.1, &velocity);
        fabrica_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    } else if (key == GLFW_KEY_A &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabrica_Vec3F velocity;

        fabrica_vec3f_scale(&s_camera_right, -0.1, &velocity);
        fabrica_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    }

    if (key == GLFW_KEY_RIGHT &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_yaw += 0.1;
        camera_recalculate_vectors();
    } else if (key == GLFW_KEY_LEFT &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_yaw -= 0.1;
        camera_recalculate_vectors();
    }

    if (key == GLFW_KEY_SPACE &&
        (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_pos.y += 0.1;
    } else if (key == GLFW_KEY_LEFT_SHIFT &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_pos.y -= 0.1;
    }
}

void handle_window_close_event(GLFWwindow *window) { s_is_running = 0; }

void handle_cursor_pos_event(GLFWwindow *window, double x, double y) {
    s_camera_yaw += (x - s_cursor_pos_x) * 0.01;
    s_camera_pitch += (y - s_cursor_pos_y) * 0.01;

    if (s_camera_pitch < TO_RADIAN(-89.0f)) {
        s_camera_pitch = TO_RADIAN(-89.0f);
    }

    if (s_camera_pitch > TO_RADIAN(89.0f)) {
        s_camera_pitch = TO_RADIAN(89.0f);
    }

    s_cursor_pos_x = x;
    s_cursor_pos_y = y;

    camera_recalculate_vectors();
}

void init_events() {
    glfwSetWindowSizeCallback(s_window, handle_window_resize_event);
    glfwSetKeyCallback(s_window, handle_key_event);
    glfwSetWindowCloseCallback(s_window, handle_window_close_event);
    glfwSetCursorPosCallback(s_window, handle_cursor_pos_event);
}

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} Buffers;

Buffers create_buffers(fabrica_ChunkMeshVertex *s_vertices, int vertices_len) {
    GLuint vao, ebo, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(fabrica_ChunkMeshVertex) * vertices_len, s_vertices,
                 GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(fabrica_ChunkMeshVertex), NULL);
    glEnableVertexAttribArray(0);

    Buffers buffers = {.vao = vao, .vbo = vbo, .ebo = ebo};

    glBindVertexArray(0);

    return buffers;
}

GLuint load_texture(const char *path) {
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (!data) {
        fprintf(stderr, "Could not load image: %s\n", path);
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

    return texture;
}

void terminate() { glfwTerminate(); }

void print_matrix(float *mat) {
    printf("[");

    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            int idx = (i * 4) + j;
            printf("\t%f ", mat[idx]);
        }
        printf("\n");
    }

    printf("]");
}

int main() {
    fabrica_Allocator default_allocator = {
        .malloc = malloc, .free = free, .realloc = realloc};

    fabrica_error_init(&default_allocator);

    init_gl();
    init_events();

    if (!fabrica_shaders_init(&default_allocator)) {
        fabrica_error_print_and_clear();
        return 1;
    }

    const fabrica_ShaderProgram *shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_CHUNK);

    GLuint u_matrix = glGetUniformLocation(shader_program->program, "u_matrix");

    float temp_matrix[16];
    float model_matrix[16];
    float view_matrix[16];
    float projection_matrix[16];
    float final_matrix[16];

    float translation_matrix[16];
    float rotation_matrix[16];

    fabrica_Vec3F dir = {0, 1, 0};
    fabrica_QuaternionF quat;

    fabrica_mat4f_persperctive(TO_RADIAN(70.0f),
                               (float)s_window_width / s_window_height, 0.5f,
                               100.0f, projection_matrix);

    fabrica_mat4f_translation(0, 0, 1, translation_matrix);
    fabrica_mat4f_identity(rotation_matrix);

    // Calculate camera angles
    fabrica_Vec3F horizontal_target = {
        .x = s_camera_front.x,
        .y = 0.0f,
        .z = s_camera_front.z,
    };
    fabrica_vec3f_normalize(&horizontal_target);

    float horizontal_angle = asinf(fabsf(horizontal_target.z));
    if (horizontal_target.z >= 0.0f) {
        if (horizontal_target.x >= 0.0f) {
            s_camera_yaw = (2 * M_PI) - horizontal_angle;
        } else {
            s_camera_yaw = M_PI + horizontal_angle;
        }

    } else {
        if (horizontal_target.x >= 0.0f) {
            s_camera_yaw = horizontal_angle;
        } else {
            s_camera_yaw = M_PI - horizontal_angle;
        }
    }

    s_camera_pitch = -asinf(s_camera_pos.y);

    glfwGetCursorPos(s_window, &s_cursor_pos_x, &s_cursor_pos_y);

    // Actually doing stuff
    fabrica_Chunk chunk;
    fabrica_chunk_init(&chunk);

    for (int x = 0; x < CHUNK_SIZE; ++x) {
        for (int y = 0; y < CHUNK_SIZE; ++y) {
            for (int z = 0; z < CHUNK_SIZE; ++z) {
                int idx = fabrica_block_index(x, y, z);
                chunk.blocks[idx].type = fabrica_BlockType_STONE;
            }
        }
    }

    fabrica_mesh_chunk(&chunk, &default_allocator);
    printf("%d %d\n", chunk.mesh.vertices_cap, chunk.mesh.vertices_len);

    Buffers buffers =
        create_buffers(chunk.mesh.vertices, chunk.mesh.vertices_len);

    while (s_is_running) {
        fabrica_mat4f_view(&s_camera_pos, &s_camera_right, &s_camera_up,
                           &s_camera_front, view_matrix);

        fabrica_quaternionf_rotate(&quat, 0.01, &dir);
        fabrica_mat4f_rotation_from_quaternionf(&quat, rotation_matrix);

        fabrica_mat4f_identity(rotation_matrix);
        fabrica_mat4f_mult(translation_matrix, rotation_matrix, model_matrix);
        fabrica_mat4f_mult(view_matrix, model_matrix, temp_matrix);
        fabrica_mat4f_mult(projection_matrix, temp_matrix, final_matrix);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        render_chunk(&chunk);

        glBindVertexArray(buffers.vao);

        glUseProgram(shader_program->program);
        glUniformMatrix4fv(u_matrix, 1, GL_TRUE, final_matrix);

        glDrawArrays(GL_TRIANGLES, 0, chunk.mesh.vertices_len);

        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    fabrica_error_terminate();
    terminate();
}
