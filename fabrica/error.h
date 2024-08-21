#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H

typedef enum {
    fabrica_Error_OK = 0,
    fabrica_Error_MEMORY_ALLOCATION_ERROR = 1
} fabrica_Error;

void fabrica_exit(fabrica_Error error);

#endif
