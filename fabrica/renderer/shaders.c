#include "fabrica/debug.h"
#include "fabrica/error.h"
#include "fabrica/file_io/read_file.h"
#include "fabrica/memory/allocator.h"

#include <fabrica/renderer/shaders.h>
#include <stdio.h>

static fabrica_ShaderProgram s_fabrica_shaders[fabrica_ShaderProgramType_COUNT];

static GLuint s_create_shader_program(const char *vertex_shader_path,
                                      const char *fragment_shader_path,
                                      const fabrica_Allocator *allocator);

fabrica_ShaderProgram *fabrica_shaders_get(fabrica_ShaderProgramType type) {
    assert(type >= 0);
    assert(type < fabrica_ShaderProgramType_COUNT);

    return &s_fabrica_shaders[type];
}

int fabrica_shaders_init(const fabrica_Allocator *allocator) {
    s_fabrica_shaders[fabrica_ShaderProgramType_CHUNK].program =
        s_create_shader_program("shaders/chunk.vert", "shaders/chunk.frag",
                                allocator);

    s_fabrica_shaders[fabrica_ShaderProgramType_TEXTURED].program =
        s_create_shader_program("shaders/textured.vert", "shaders/textured.frag",
                                allocator);

    for (int i = 0; i < fabrica_ShaderProgramType_COUNT; ++i) {
        if (!s_fabrica_shaders[i].program) {
            return 0;
        }
    }

    return 1;
}

void fabrica_shaders_terminate() {
    for (int i = 0; i < fabrica_ShaderProgramType_COUNT; ++i) {
        glDeleteProgram(s_fabrica_shaders[i].program);
    }
}

static GLuint s_create_shader_program(const char *vertex_shader_path,
                                      const char *fragment_shader_path,
                                      const fabrica_Allocator *allocator) {
    GLchar *vertex_shader_src =
        fabrica_read_file_string(vertex_shader_path, allocator);
    if (!vertex_shader_src) {
        return 0;
    }

    GLchar *fragment_shader_src =
        fabrica_read_file_string(fragment_shader_path, allocator);
    if (!fragment_shader_src) {
        return 0;
    }

    // Compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, (const GLchar *const *)&vertex_shader_src,
                   0);
    glCompileShader(vertex_shader);

    GLint vertex_shader_compiled;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &vertex_shader_compiled);

    if (vertex_shader_compiled == 0) {
        int maxLength = 255;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &maxLength);

        char *vertexInfoLog = allocator->malloc(maxLength);
        glGetShaderInfoLog(vertex_shader, maxLength, &maxLength, vertexInfoLog);

        fabrica_error_push_message(
            fabrica_ErrorCode_RENDERER_SHADER_COMPILATION, vertexInfoLog);
        allocator->free(vertexInfoLog);

        return 0;
    }

    // Compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1,
                   (const GLchar *const *)&fragment_shader_src, 0);
    glCompileShader(fragment_shader);

    GLint fragment_shader_compiled;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS,
                  &fragment_shader_compiled);

    if (fragment_shader_compiled == 0) {
        int maxLength = 255;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &maxLength);

        char *fragmentInfoLog = allocator->malloc(maxLength);
        glGetShaderInfoLog(fragment_shader, maxLength, &maxLength,
                           fragmentInfoLog);

        fabrica_error_push_message(
            fabrica_ErrorCode_RENDERER_SHADER_COMPILATION, fragmentInfoLog);
        allocator->free(fragmentInfoLog);

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
        char *info_log = allocator->malloc(512);

        glGetProgramInfoLog(program, 512, NULL, info_log);

        fabrica_error_push_message(
            fabrica_ErrorCode_RENDERER_SHADER_PROGRAM_LINKING, info_log);
        allocator->free(info_log);

        return 0;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}
