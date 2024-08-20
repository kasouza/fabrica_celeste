#include "fabrica/math/quaternionf.h"
#include "fabrica/math/vec3f.h"
#include <KHR/khrplatform.h>
#include <assert.h>
#include <fabrica/math/mat4f.h>
#include <math.h>
#include <stdio.h>

#include <stb/stb_image.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <stdlib.h>
#include <time.h>

#define TO_RADIAN(x) (float)(((x)*M_PI / 180.0f))

static GLFWwindow *s_window = NULL;
static int s_window_width = 800;
static int s_window_height = 600;

static int s_is_running = 1;

static fabricam_Vec3F s_camera_pos = {0};

static float s_camera_yaw = M_PI / 2;
static float s_camera_pitch = 0.0f;

static double s_cursor_pos_x;
static double s_cursor_pos_y;

static fabricam_Vec3F s_camera_right = {
    1.0f,
    0.0f,
    0.0f,
};

static fabricam_Vec3F s_camera_up = {
    0.0f,
    1.0f,
    0.0f,
};

static fabricam_Vec3F s_camera_front = {
    0.0f,
    0.0f,
    1.0f,
};

static const char *s_vertex_shader_src =
    "#version 330\n"
    "layout (location = 0) in vec3 a_pos;\n"
    "layout (location = 1) in vec2 a_tex_coords;\n"
    "uniform mat4 u_matrix;\n"
    "out vec2 tex_coords;\n"
    "void main() {\n"
    "    gl_Position = u_matrix * vec4(a_pos, 1.0);\n"
    "    tex_coords = a_tex_coords;\n"
    "}";

static const char *s_fragment_shader_src =
    "#version 330\n"
    "in vec2 tex_coords;\n"
    "uniform sampler2D text;\n"
    "out vec4 frag_color;\n"
    "void main() {\n"
    "    frag_color = texture(text, tex_coords);\n"
    "}";

// Cube mesh data
static GLfloat s_vertices[] = {
    1.0f, 1.0f, 1.0f, 1.0, 1.0,
    0.0f, 1.0f, 0.0f, 0.0, 0.0,
    0.0f, 1.0f, 1.0f, 0.0, 1.0,
    1.0f, 0.0f, 0.0f, 1.0, 1.0,
    0.0f, 0.0f, 0.0f, 0.0, 1.0,
    1.0f, 1.0f, 0.0f, 1.0, 0.0,
    1.0f, 0.0f, 1.0f, 1.0, 0.0,
    0.0f, 0.0f, 1.0f, 0.0, 0.0,
};

static GLuint s_indices[36] = {0, 1, 2, 1, 3, 4, 5, 6, 3, 7, 3, 6,
                               2, 4, 7, 0, 7, 6, 0, 5, 1, 1, 5, 3,
                               5, 0, 6, 7, 4, 3, 2, 1, 4, 0, 2, 7};

void init_gl() {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    glfwGetMonitorWorkarea(monitor, NULL, NULL, &s_window_width,
                           &s_window_height);

    s_window = glfwCreateWindow(s_window_width, s_window_height, "Mine",
                                monitor, NULL);
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
    fabricam_Vec3F world_up = {0.0f, 1.0f, 0.0f};
    fabricam_Vec3F front = {1.0f, 0.0f, 0.0f};
    fabricam_vec3f_rotate(&front, s_camera_yaw, &world_up);
    fabricam_vec3f_normalize(&front);

    fabricam_Vec3F right;
    fabricam_vec3f_cross_product(&world_up, &front, &right);
    fabricam_vec3f_normalize(&right);

    fabricam_vec3f_rotate(&front, s_camera_pitch, &right);
    fabricam_vec3f_normalize(&front);

    fabricam_Vec3F up;
    fabricam_vec3f_cross_product(&front, &right, &up);

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
        fabricam_Vec3F velocity;

        fabricam_vec3f_scale(&s_camera_front, 0.1, &velocity);
        fabricam_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    } else if (key == GLFW_KEY_S &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabricam_Vec3F velocity;

        fabricam_vec3f_scale(&s_camera_front, -0.1, &velocity);
        fabricam_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    }

    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabricam_Vec3F velocity;

        fabricam_vec3f_scale(&s_camera_right, 0.1, &velocity);
        fabricam_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
    } else if (key == GLFW_KEY_A &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        fabricam_Vec3F velocity;

        fabricam_vec3f_scale(&s_camera_right, -0.1, &velocity);
        fabricam_vec3f_add(&s_camera_pos, &velocity, &s_camera_pos);
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

    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_pitch -= 0.1;
        camera_recalculate_vectors();
    } else if (key == GLFW_KEY_DOWN &&
               (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        s_camera_pitch += 0.1;
        camera_recalculate_vectors();
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

GLuint create_shader_program() {
    // Create vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &s_vertex_shader_src, 0);
    glCompileShader(vertex_shader);

    GLint vertex_shader_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compiled);
    if (vertex_shader_compiled == 0) {
        int maxLength = 255;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

        /* The maxLength includes the NULL character */
        char *vertexInfoLog = (char *)malloc(maxLength);

        glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, vertexInfoLog);

        fprintf(stderr, "Error compiling vertex shader: \n%s\n", vertexInfoLog);
        free(vertexInfoLog);

        return 1;
    }

    // Create fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &s_fragment_shader_src, 0);
    glCompileShader(fragment_shader);

    GLint fragment_shader_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS,
                  &fragment_shader_compiled);

    if (fragment_shader_compiled == 0) {
        int maxLength = 255;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

        /* The maxLength includes the NULL character */
        char *fragmentInfoLog = (char *)malloc(maxLength);

        glGetShaderInfoLog(fragment_shader, maxLength, &maxLength,
                           fragmentInfoLog);

        fprintf(stderr, "Error compiling fragment shader: \n%s\n",
                fragmentInfoLog);
        free(fragmentInfoLog);

        return 0;
    }

    // Create program
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    GLint program_linked;
    glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
    if (program_linked == 0) {
        /* The maxLength includes the NULL character */
        char *info_log = (char *)malloc(512);

        glGetProgramInfoLog(program, 512, NULL, info_log);

        fprintf(stderr, "%s\n", info_log);
        free(info_log);

        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}

typedef struct {
    GLuint vao;
    GLuint vbo;
    GLuint ebo;
} Buffers;

Buffers create_buffers() {
    GLuint vao, ebo, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s_indices), s_indices,
                 GL_STATIC_DRAW);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_vertices), s_vertices,
                 GL_STATIC_DRAW);

    /*glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), NULL);*/
    /*glEnableVertexAttribArray(0);*/

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), NULL);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

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
    init_gl();
    init_events();

    GLuint shader_program = create_shader_program();
    if (!shader_program) {
        return 1;
    }

    GLuint u_matrix = glGetUniformLocation(shader_program, "u_matrix");
    Buffers buffers = create_buffers();

    GLuint texture = load_texture("assets/container.jpg");

    float temp_matrix[16];
    float model_matrix[16];
    float view_matrix[16];
    float projection_matrix[16];
    float final_matrix[16];

    float translation_matrix[16];
    float rotation_matrix[16];

    fabricam_Vec3F dir = {0, 1, 0};
    fabricam_QuaternionF quat;

    fabricam_mat4f_persperctive(TO_RADIAN(70.0f),
                                (float)s_window_width / s_window_height, 0.5f,
                                100.0f, projection_matrix);

    fabricam_mat4f_translation(0, 0, 1, translation_matrix);
    fabricam_mat4f_identity(rotation_matrix);

    // Calculate camera angles
    fabricam_Vec3F horizontal_target = {
        .x = s_camera_front.x,
        .y = 0.0f,
        .z = s_camera_front.z,
    };
    fabricam_vec3f_normalize(&horizontal_target);

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

    while (s_is_running) {
        fabricam_mat4f_view(&s_camera_pos, &s_camera_right, &s_camera_up,
                            &s_camera_front, view_matrix);

        fabricam_quaternionf_rotate(&quat, 0.01, &dir);
        fabricam_mat4f_rotation_from_quaternionf(&quat, rotation_matrix);

        fabricam_mat4f_identity(rotation_matrix);
        fabricam_mat4f_mult(translation_matrix, rotation_matrix, model_matrix);
        fabricam_mat4f_mult(view_matrix, model_matrix, temp_matrix);
        fabricam_mat4f_mult(projection_matrix, temp_matrix, final_matrix);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(buffers.vao);
        glActiveTexture(GL_TEXTURE0);

        glUseProgram(shader_program);
        glUniformMatrix4fv(u_matrix, 1, GL_TRUE, final_matrix);

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(s_window);
        glfwPollEvents();
    }

    terminate();
}
