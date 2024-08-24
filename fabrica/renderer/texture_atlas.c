#include "fabrica/renderer/texture_atlas.h"
#include "fabrica/error.h"
#include "fabrica/file_io/image.h"
#include "fabrica/renderer/texture.h"

#include "fabrica/debug.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int calculate_next_square(int number) {
    int next = 1;
    while (next < number) {
        next *= 2;
    }

    return next;
}

fabrica_ErrorCode
fabrica_texture_atlas_init(fabrica_TextureAtlas *atlas, int image_size,
                           int channels, const fabrica_Image *images,
                           int images_len, const fabrica_Allocator *allocator) {
    assert(atlas != NULL);
    assert(image_size > 0);

    assert(((image_size & (image_size - 1)) == 0) &&
           "image_size must be a power of 2");

    for (int i = 0; i < images_len; i++) {
        if (images[i].width % image_size != 0 ||
            images[i].height % image_size != 0) {
            fabrica_error_push(fabrica_ErrorCode_TEXTURE_ATLAS_IMAGE_SIZE);
            return fabrica_ErrorCode_TEXTURE_ATLAS_IMAGE_SIZE;
        }

        if (images[i].channels != channels) {
            fabrica_error_push(fabrica_ErrorCode_TEXTURE_ATLAS_CHANNELS);
            return fabrica_ErrorCode_TEXTURE_ATLAS_CHANNELS;
        }
    }

    int number_of_indices = 0;

    for (int i = 0; i < images_len; i++) {
        const fabrica_Image *image = &images[i];

        int res_x = image->width / image_size;
        int res_y = image->height / image_size;

        number_of_indices += res_x * res_y;
    }

    int min_atlas_size_indices = ceil(sqrt((float)number_of_indices));

    int min_size_pixels = min_atlas_size_indices * image_size;
    int atlas_size_pixels = calculate_next_square(min_size_pixels);
    int atlas_size_indices = atlas_size_pixels / image_size;

    fabrica_AtlasIndices *indices =
        allocator->malloc(sizeof(fabrica_AtlasIndices) * images_len);

    if (indices == NULL) {
        fabrica_error_push(fabrica_ErrorCode_MEMORY_ALLOCATION);
        return fabrica_ErrorCode_MEMORY_ALLOCATION;
    }

    unsigned char *atlas_data =
        allocator->malloc(sizeof(unsigned char) * atlas_size_pixels *
                          atlas_size_pixels * channels);
    if (atlas_data == NULL) {
        fabrica_error_push(fabrica_ErrorCode_MEMORY_ALLOCATION);
        return fabrica_ErrorCode_MEMORY_ALLOCATION;
    }

    memset(atlas_data, 0,
           sizeof(unsigned char) * atlas_size_pixels * atlas_size_pixels *
               channels);

    int current_index = 0;

    for (int i = 0; i < images_len; i++) {
        const fabrica_Image *image = &images[i];

        int res_x = image->width / image_size;
        int res_y = image->height / image_size;

        indices[i].first = current_index;
        indices[i].last = (current_index + (res_x * res_y)) - 1;
        // Iterate through each chunk/index of the image
        for (int image_index_x = 0; image_index_x < res_y; image_index_x++) {
            for (int image_index_y = 0; image_index_y < res_x;
                 image_index_y++) {
                int index = current_index;
                current_index++;

                int atlas_index_x = index % atlas_size_indices;
                int atlas_index_y = index / atlas_size_indices;

                // Flip the atlas y
                /*atlas_index_y = atlas_size_indices - atlas_index_y - 1;*/

                int atlas_first_pixel_x = atlas_index_x * image_size;
                int atlas_first_pixel_y = atlas_index_y * image_size;

                int image_first_pixel_x = image_index_y * image_size;
                int image_first_pixel_y = image_index_x * image_size;

                // Iterate through each pixel in the image
                for (int image_pixel_x = 0; image_pixel_x < image_size;
                     image_pixel_x++) {
                    for (int image_pixel_y = 0; image_pixel_y < image_size;
                         image_pixel_y++) {
                        int target_pixel_x =
                            atlas_first_pixel_x + image_pixel_x;
                        int target_pixel_y =
                            atlas_first_pixel_y + image_pixel_y;

                        int atlas_pixel_first_index =
                            ((target_pixel_y * atlas_size_pixels) +
                             target_pixel_x) *
                            channels;

                        int source_pixel_x =
                            image_first_pixel_x + image_pixel_x;
                        int source_pixel_y =
                            image_first_pixel_y + image_pixel_y;

                        int image_pixel_first_index =
                            ((source_pixel_y * image->width) + source_pixel_x) *
                            channels;

                        // Copy all the channels for the pixel
                        for (int channel = 0; channel < channels; channel++) {
                            atlas_data[atlas_pixel_first_index + channel] =
                                image->data[image_pixel_first_index + channel];
                        }
                    }
                }
            }
        }
    }

    fabrica_Image atlas_image;
    fabrica_image_from_memory(&atlas_image, atlas_data, atlas_size_pixels,
                              atlas_size_pixels, channels);

    fabrica_texture_init(&atlas->texture, &atlas_image);

    atlas->size_pixels = atlas_size_pixels;
    atlas->size_indices = atlas_size_indices;
    atlas->image_size_pixels = image_size;
    atlas->indices = indices;
    atlas->indices_len = images_len;
    atlas->allocator = allocator;

    fabrica_image_destroy(&atlas_image);

    return fabrica_ErrorCode_OK;
}
