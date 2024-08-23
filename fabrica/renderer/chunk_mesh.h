#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_CHUNK_MESH_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_CHUNK_MESH_H

#include <fabrica/memory/allocator.h>
#include <fabrica/memory/malloc.h>
#include <fabrica/renderer/gl.h>

typedef struct fabrick_Chunk fabrica_Chunk;
typedef struct {
    GLfloat x;
    GLfloat y;
    GLfloat z;
} fabrica_ChunkMeshVertex;

typedef struct {
    fabrica_ChunkMeshVertex *vertices;
    float transformation_matrix[16];
    int vertices_len;
    int vertices_cap;

    const fabrica_Allocator *allocator;
} fabrica_ChunkMesh;

void fabrica_chunk_mesh_init(fabrica_ChunkMesh *chunk_mesh,
                             const fabrica_Allocator *allocator);
void fabrica_chunk_mesh_build(fabrica_Chunk *chunk);

#endif
