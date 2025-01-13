#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_SHADERS_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_SHADERS_H

#include "fabrica/renderer/gl.h"

typedef enum {
    fabrica_ShaderProgramType_CHUNK = 0,
    fabrica_ShaderProgramType_TEXTURED,
    fabrica_ShaderProgramType_HIGHLIGHT,

    fabrica_ShaderProgramType_COUNT
} fabrica_ShaderProgramType;

typedef struct {
    GLuint program;
} fabrica_ShaderProgram;

fabrica_ShaderProgram *fabrica_shaders_get(fabrica_ShaderProgramType type);

/**
 * Returns zero on failure
 */
int fabrica_shaders_init();

void fabrica_shaders_terminate();

#endif
