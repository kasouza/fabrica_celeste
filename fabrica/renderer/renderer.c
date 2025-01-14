#include "fabrica/renderer/renderer.h"
#include "fabrica/glfw.h"
#include "fabrica/math/mat4f.h"
#include "fabrica/renderer/chunk_mesh.h"
#include "fabrica/renderer/gl.h"
#include "fabrica/renderer/shaders.h"
#include "fabrica/renderer/texture.h"
#include "fabrica/utils/constants.h"
#include "fabrica/world/chunk.h"
#include "fabrica/world/raycast.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TO_RADIAN(x) (float)(((x) * M_PI / 180.0f))
#define NEAR 1.0f
#define FAR 100.0f

static GLuint s_chunk_vao = 0;
static GLuint s_chunk_vbo = 0;

static GLuint s_highlight_vao = 0;
static GLuint s_highlight_vbo = 0;
static GLuint s_highlight_ebo = 0;

void fabrica_render_chunk(const fabrica_Chunk *chunk);
void default_perspective_matrix(float *mat);

bool fabrica_renderer_init() {
    if (!fabrica_shaders_init()) {
        return false;
    }

    // Highlight
    GLfloat highlight_vertices[24] = {
        0.0f,        0.0f,        0.0f, // 0 Front Bottom Left
        0.0f + 1.0f, 0.0f,        0.0f, // 1 Front Bottom Right
        0.0f + 1.0f, 0.0f + 1.0f, 0.0f, // 2 Front Top Right
        0.0f,        0.0f + 1.0f, 0.0f, // 3 Front Top Left

        0.0f,        0.0f,        0.0f + 1.0f, // 4 Back Bottom Left
        0.0f + 1.0f, 0.0f,        0.0f + 1.0f, // 5 Back Bottom Right
        0.0f + 1.0f, 0.0f + 1.0f, 0.0f + 1.0f, // 6 Back Top Right
        0.0f,        0.0f + 1.0f, 0.0f + 1.0f, // 7 Back Top Left
    };

    GLuint highlight_indices[24] = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                                    6, 7, 7, 4, 0, 4, 1, 5, 3, 7, 2, 6};

    glGenVertexArrays(1, &s_highlight_vao);
    glBindVertexArray(s_highlight_vao);

    glGenBuffers(1, &s_highlight_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_highlight_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(highlight_vertices),
                 highlight_vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, NULL);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &s_highlight_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_highlight_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(highlight_indices),
                 highlight_indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    // Chunk
    glGenVertexArrays(1, &s_chunk_vao);
    glBindVertexArray(s_chunk_vao);

    glGenBuffers(1, &s_chunk_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, s_chunk_vbo);

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

void fabrica_render(fabrica_World *world, const fabrica_Camera *camera,
                    const fabrica_TextureAtlas *atlas) {
    assert(world != NULL);
    assert(camera != NULL);
    assert(atlas != NULL);

    float view_matrix[16];
    float projection_matrix[16];
    float temp_matrix[16];
    float final_matrix[16];

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    fabrica_camera_view_matrix(camera, view_matrix);
    default_perspective_matrix(projection_matrix);

    // Render chunks
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0, 1.0);
    glDepthFunc(GL_LESS);

    const fabrica_ShaderProgram *chunk_shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_CHUNK);

    fabrica_Chunk **all_chunks;
    int all_chunks_len = 0;

    fabrica_chunk_map_get_all(&world->chunks, &all_chunks, &all_chunks_len);

    glUseProgram(chunk_shader_program->program);
    glBindVertexArray(s_chunk_vao);

    for (int i = 0; i < all_chunks_len; ++i) {
        // TODO: Move the mesh building to separate threads
        if (all_chunks[i]->is_dirty) {
            fabrica_chunk_mesh_build(all_chunks[i], atlas);
            all_chunks[i]->is_dirty = false;
        }

        const fabrica_ChunkMesh *mesh = &all_chunks[i]->mesh;
        assert(mesh != NULL);

        fabrica_mat4f_mult(view_matrix, mesh->transformation_matrix,
                           temp_matrix);
        fabrica_mat4f_mult(projection_matrix, temp_matrix, final_matrix);

        glBindBuffer(GL_ARRAY_BUFFER, s_chunk_vbo);
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

    free(all_chunks);
    glDisable(GL_POLYGON_OFFSET_FILL);

    // Render selected block highligth
    glDepthFunc(GL_LEQUAL);

    fabrica_RaycastHit hit;
    fabrica_raycast(world, &camera->pos, &camera->front, &hit,
                    FABRICA_PLAYER_INTERACTION_MAX_LENGTH);

    if (hit.hit) {
        const fabrica_ShaderProgram *highlight_shader_program =
            fabrica_shaders_get(fabrica_ShaderProgramType_HIGHLIGHT);

        glUseProgram(highlight_shader_program->program);
        glBindVertexArray(s_highlight_vao);

        GLuint u_color =
            glGetUniformLocation(highlight_shader_program->program, "u_color");
        glUniform3f(u_color, 1.0f, 0.0f, 0.0f);

        float translation[16];
        fabrica_mat4f_translation(hit.block_pos.x, hit.block_pos.y,
                                  hit.block_pos.z, translation);
        fabrica_mat4f_mult(view_matrix, translation, temp_matrix);
        fabrica_mat4f_mult(projection_matrix, temp_matrix, final_matrix);

        GLuint u_matrix =
            glGetUniformLocation(highlight_shader_program->program, "u_matrix");
        glUniformMatrix4fv(u_matrix, 1, GL_TRUE, final_matrix);

        glLineWidth(4);
        glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, NULL);
        glLineWidth(1);
    }

    glfwSwapBuffers(fabrica_glfw_get_window());
    glfwPollEvents();
}

void default_perspective_matrix(float *mat) {
    int width, height;
    fabrica_glfw_get_window_dimensions(&width, &height);

    fabrica_mat4f_persperctive(TO_RADIAN(70.0f), (float)width / height, 0.5f,
                               100.0f, mat);
}
