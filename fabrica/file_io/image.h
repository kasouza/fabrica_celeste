#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_FILE_IO_IMAGE_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_FILE_IO_IMAGE_H

typedef struct {
    unsigned char *data;
    int width;
    int height;
    int channels;
} fabrica_Image;

void fabrica_image_load(fabrica_Image *out_image, const char *path);
void fabrica_image_from_memory(fabrica_Image *out_image, const unsigned char *data, int width, int height, int channels);
void fabrica_image_destroy(fabrica_Image *image);

#endif
