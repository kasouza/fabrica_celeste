#include "fabrica/error.h"
#include "fabrica/file_io/image.h"
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
#include "fabrica/renderer/texture.h"
#include "fabrica/renderer/texture_atlas.h"
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

    if (!fabrica_renderer_init(&default_allocator)) {
        fabrica_error_print_and_clear();
        return 1;
    }

    fabrica_Image images[2];
    fabrica_image_load(&images[0], "assets/16x16.png");
    fabrica_image_load(&images[1], "assets/32x32.png");
    /*fabrica_image_load(&images[2], "assets/3.png");*/
    /*fabrica_image_load(&images[3], "assets/4.png");*/
    /*fabrica_image_load(&images[4], "assets/5.png");*/

    /*fabrica_image_load(&images[5], "assets/1.png");*/
    /*fabrica_image_load(&images[6], "assets/2.png");*/
    /*fabrica_image_load(&images[7], "assets/3.png");*/
    /*fabrica_image_load(&images[8], "assets/4.png");*/

    /*fabrica_image_load(&images[9], "assets/1.png");*/
    /*fabrica_image_load(&images[10], "assets/2.png");*/
    /*fabrica_image_load(&images[11], "assets/3.png");*/
    /*fabrica_image_load(&images[12], "assets/2.png");*/
    /*fabrica_image_load(&images[12], "assets/5.png");*/

    /*fabrica_image_load(&images[13], "assets/1.png");*/
    /*fabrica_image_load(&images[14], "assets/2.png");*/

    fabrica_TextureAtlas atlas;
    fabrica_ErrorCode error =
        fabrica_texture_atlas_init(&atlas, 16, 4, images, 2, &default_allocator);
    if (error != fabrica_ErrorCode_OK) {
        fabrica_error_print_and_clear();
        return 1;
    }

    for (int i = 0; i < atlas.indices_len; i++) {
        printf("i: %d - first: %d, last: %d\n", i, atlas.indices[i].first,
               atlas.indices[i].last);
    }

    init_events(fabrica_renderer_get_window());

    fabrica_camera_init(&s_camera, (fabrica_Vec3F){0.0f, 0.0f, -1.0f},
                        (fabrica_Vec3F){0.0f, 0.0f, 1.0f});

    glfwGetCursorPos(fabrica_renderer_get_window(), &s_cursor_pos_x,
                     &s_cursor_pos_y);

    fabrica_World world;
    fabrica_world_init(&world, &default_allocator);

    fabrica_ShaderProgram *shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_TEXTURED);

    int idx = 1;
    int size = atlas.size_indices;
    int resolution = atlas.image_size_pixels;

    int x = (idx % size) * resolution;
    int y = (idx / size) * resolution;

    int x2 = x + resolution;
    int y2 = y + resolution;

    float uv_x = (float)x / atlas.size_pixels;
    float uv_y = (float)y / atlas.size_pixels;

    float uv_x2 = (float)x2 / atlas.size_pixels;
    float uv_y2 = (float)y2 / atlas.size_pixels;

    /*float vertices[] = {*/
        /*// positions          // colors           // texture coords*/
        /*0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, uv_x2, uv_y2, // top right*/
        /*0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, uv_x2, uv_y,  // bottom right*/
        /*-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, uv_x,  uv_y,  // bottom left*/
        /*-0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, uv_x,  uv_y2  // top left*/
    /*};*/

    float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture
    // -------------------------
    unsigned int texture1;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    GL_REPEAT); // set texture wrapping to GL_REPEAT (default
                                // wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(
        true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can
    // find files on any IDE/platform; replace it with your own image path.
    unsigned char *data =
        stbi_load("assets/container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        fprintf(stderr, "Failed to load texture\n");
    }
    stbi_image_free(data);

    glUseProgram(shader_program->program);
    glUniform1i(glGetUniformLocation(shader_program->program, "texture1"), 0);

    fabrica_Texture texture;
    fabrica_texture_load(&texture, "assets/container.jpg");

    while (s_is_running) {
        /*fabrica_render(&world, &s_camera);*/

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas.texture.id);

        // render container
        glUseProgram(shader_program->program);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwPollEvents();
        glfwSwapBuffers(fabrica_renderer_get_window());
    }

    fabrica_error_terminate();
    glfwTerminate();
}
