#include "fabrica/renderer/renderer.h"
#include "fabrica/error.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/memory/allocator.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/renderer/shaders.h"
#include "fabrica/renderer/texture.h"
#include "fabrica/world/chunk.h"

#include <GLFW/glfw3.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define TO_RADIAN(x) (float)(((x)*M_PI / 180.0f))
#define NEAR 1.0f
#define FAR 100.0f

static GLFWwindow *s_window = NULL;
static int s_window_width = 800;
static int s_window_height = 600;

static GLuint s_vao = 0;
static GLuint s_vbo = 0;

void fabrica_render_chunk(const fabrica_Chunk *chunk);
void init_gl();
void handle_window_resize_event(GLFWwindow *window, int width, int height);

bool fabrica_renderer_init(const fabrica_Allocator *allocator) {
    assert(allocator != NULL);

    init_gl();

    if (!fabrica_shaders_init(allocator)) {
        return false;
    }

    glGenVertexArrays(1, &s_vao);
    glBindVertexArray(s_vao);

    glGenBuffers(1, &s_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          sizeof(fabrica_ChunkMeshVertex), NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          sizeof(fabrica_ChunkMeshVertex),
                          (void *)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return true;
}

void fabrica_render(const fabrica_World *world, const fabrica_Camera *camera,
                    const fabrica_TextureAtlas *atlas) {
    float view_matrix[16];
    float projection_matrix[16];
    float temp_matrix[16];
    float final_matrix[16];

    fabrica_camera_view_matrix(camera, view_matrix);
    fabrica_mat4f_persperctive(TO_RADIAN(70.0f),
                               (float)s_window_width / s_window_height, 0.5f,
                               100.0f, projection_matrix);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    const fabrica_ShaderProgram *chunk_shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_CHUNK);

    for (int i = 0; i < world->chunks_len; ++i) {
        const fabrica_ChunkMesh *mesh = &world->chunks[i].mesh;

        fabrica_mat4f_mult(view_matrix, mesh->transformation_matrix,
                           temp_matrix);
        fabrica_mat4f_mult(projection_matrix, temp_matrix, final_matrix);

        glUseProgram(chunk_shader_program->program);

        glBindVertexArray(s_vao);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(fabrica_ChunkMeshVertex) * mesh->vertices_len,
                     mesh->vertices, GL_DYNAMIC_DRAW);

        GLuint u_matrix =
            glGetUniformLocation(chunk_shader_program->program, "u_matrix");
        glUniformMatrix4fv(u_matrix, 1, GL_TRUE, final_matrix);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas->texture.id);

        GLuint u_texture =
            glGetUniformLocation(chunk_shader_program->program, "u_texture");
        glUniform1i(u_texture, 0);

        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices_len);
    }

    glfwSwapBuffers(s_window);
    glfwPollEvents();
}

GLFWwindow *fabrica_renderer_get_window() { return s_window; }

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

    glfwSetWindowSizeCallback(s_window, handle_window_resize_event);
}

void handle_window_resize_event(GLFWwindow *window, int width, int height) {
    s_window_width = width;
    s_window_height = height;

    glViewport(0, 0, width, height);
}
