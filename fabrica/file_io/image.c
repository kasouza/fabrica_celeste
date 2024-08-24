#include "fabrica/file_io/image.h"

#include "fabrica/debug.h"
#include "fabrica/error.h"

#include <stb/stb_image.h>

#include <stdbool.h>
#include <stddef.h>

void fabrica_image_load(fabrica_Image *out_image, const char *path) {
    assert(path != NULL);
    assert(out_image != NULL);

    stbi_set_flip_vertically_on_load(true);
    out_image->data = stbi_load(path, &out_image->width, &out_image->height,
                                &out_image->channels, 0);
    if (!out_image->data) {
        fabrica_error_push_message(fabrica_ErrorCode_IMAGE_LOAD, path);
    }
}

void fabrica_image_from_memory(fabrica_Image *out_image,
                               const unsigned char *data, int width, int height,
                               int channels) {
    assert(data != NULL);
    assert(out_image != NULL);
    assert(width > 0);
    assert(height > 0);
    assert(channels > 0);

    out_image->data = (unsigned char *)data;
    out_image->width = width;
    out_image->height = height;
    out_image->channels = channels;
}

void fabrica_image_destroy(fabrica_Image *image) {
    assert(image != NULL);

    if (image->data != NULL) {
        stbi_image_free(image->data);
        image->data = NULL;
    }
}
