#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_TEXTURE_ATLAS_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_TEXTURE_ATLAS_H

#include "fabrica/error.h"
#include "fabrica/file_io/image.h"
#include "fabrica/memory/allocator.h"
#include "fabrica/renderer/texture.h"

typedef struct {
    int first;
    int last;
} fabrica_AtlasIndices;

typedef struct {
    /** The texture atlas*/
    fabrica_Texture texture;

    /** The width and height of the generated texture atlas in pixels*/
    int size_pixels;

    /** The number of indices per row/column*/
    int size_indices;

    /** The width and height of each image in the atlas in pixels*/
    int image_size_pixels;

    /**
     * The indices to the images used to generate the texture atlas.
     * The order will be the same as the order of the images passed to
     * fabrica_texture_atlas_init function.
     */
    fabrica_AtlasIndices *indices;
    int indices_len;

    const fabrica_Allocator *allocator;
} fabrica_TextureAtlas;

/**
 * The indices for the images will be stored in the same order as the images
 * passed to this function.
 *
 * Ex:
 * // The first texture is a 16x16 image
 * // The second texture is a 32x32 image
 * fabrica_Image images[2];
 * initialize_textures(&images, 2); // Fill the array with images
 *
 * fabrica_TextureAtlas atlas;
 * fabrica_texture_atlas_init(&atlas, 16, 4, images, 2, &default_allocator);
 * 
 * // The indices for the first image can be found at atlas.indices[0]
 * printf("%d\n", atlas.indices[0].first); // 0
 * printf("%d\n", atlas.indices[0].last); // 0
 * 
 * // The indices for the second image can be found at atlas.indices[1]
 * printf("%d\n", atlas.indices[1].first); // 1
 * printf("%d\n", atlas.indices[1].last); // 4
 * 
 */
fabrica_ErrorCode
fabrica_texture_atlas_init(fabrica_TextureAtlas *atlas, int image_size,
                           int channels, const fabrica_Image *images,
                           int images_len, const fabrica_Allocator *allocator);
void fabrica_texture_atlas_destroy(fabrica_TextureAtlas *atlas);

fabrica_ErrorCode fabrica_texture_atlas_load_image(fabrica_TextureAtlas *atlas,
                                                   const char *path);
void fabrica_texture_atlas_build(fabrica_TextureAtlas *atlas);
void fabrica_texture_atlas_destroy(fabrica_TextureAtlas *atlas);

#endif
