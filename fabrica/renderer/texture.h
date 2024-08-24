#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_TEXTURE_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_TEXTURE_H

#include "fabrica/file_io/image.h"
#include "fabrica/renderer/gl.h"
#include <stdbool.h>

typedef struct {
    GLuint id;
    int width;
    int height;
    int channels;
} fabrica_Texture;

void fabrica_texture_init(fabrica_Texture *texture, const fabrica_Image *image);
void fabrica_texture_load(fabrica_Texture *out_texture, const char *path);

#endif
