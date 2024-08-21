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
    int vertices_len;
    int vertices_cap;
} fabrica_ChunkMesh;

void fabrica_chunk_mesh_init(fabrica_ChunkMesh *chunk_mesh);

void fabrica_mesh_chunk(fabrica_Chunk *chunk,
                        const fabrica_Allocator *allocator);

#endif
