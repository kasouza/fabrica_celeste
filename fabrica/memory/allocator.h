#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_MEMORY_ALLOCATOR_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_MEMORY_ALLOCATOR_H

#include "fabrica/memory/free.h"
#include "fabrica/memory/malloc.h"
#include "fabrica/memory/realloc.h"
typedef struct {
    fabrica_realloc_func realloc;
    fabrica_malloc_func malloc;
    fabrica_free_func free;
} fabrica_Allocator;

#endif
