#include "fabrica/renderer/texture.h"
#include "fabrica/error.h"

#include <assert.h>
#include <stb/stb_image.h>

void fabrica_texture_init(fabrica_Texture *out_texture,
                          const fabrica_Image *image) {
    assert(image != NULL);
    assert(out_texture != NULL);

    out_texture->id = 0;
    out_texture->width = image->width;
    out_texture->height = image->height;
    out_texture->channels = image->channels;

    GLenum format;
    switch (image->channels) {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        fabrica_error_push(fabrica_ErrorCode_RENDERER_TEXTURE_LOAD_FORMAT);
        return;
    }

    glGenTextures(1, &out_texture->id);
    glBindTexture(GL_TEXTURE_2D, out_texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_texture->width,
                 out_texture->height, 0, format, GL_UNSIGNED_BYTE, image->data);
}

void fabrica_texture_load(fabrica_Texture *out_texture, const char *path) {
    assert(path != NULL);
    assert(out_texture != NULL);

    out_texture->id = 0;
    out_texture->width = 0;
    out_texture->height = 0;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data =
        stbi_load(path, &out_texture->width, &out_texture->height,
                  &out_texture->channels, 0);
    if (!data) {
        fabrica_error_push_message(fabrica_ErrorCode_RENDERER_TEXTURE_LOAD,
                                   path);
        return;
    }

    GLenum format;
    switch (out_texture->channels) {
    case 1:
        format = GL_RED;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
        format = GL_RGBA;
        break;
    default:
        fabrica_error_push_message(
            fabrica_ErrorCode_RENDERER_TEXTURE_LOAD_FORMAT, path);
        stbi_image_free(data);
        return;
    }

    glGenTextures(1, &out_texture->id);
    glBindTexture(GL_TEXTURE_2D, out_texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, out_texture->width,
                 out_texture->height, 0, format, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
}
