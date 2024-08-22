#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H

#include "fabrica/memory/allocator.h"
typedef enum {
    fabrica_ErrorCode_OK = 0,
    fabrica_ErrorCode_MEMORY_ALLOCATION = 1,
    fabrica_ErrorCode_READ_FILE = 2
} fabrica_ErrorCode;

typedef struct {
    fabrica_ErrorCode code;
    int errno;
} fabrica_Error;

void fabrica_error_init(const fabrica_Allocator *allocator);
void fabrica_error_terminate();

void fabrica_error_push(fabrica_ErrorCode code);
void fabrica_error_push_errno(fabrica_ErrorCode code, int errno);

const fabrica_Error *fabrica_error_pop();

void fabrica_exit(fabrica_ErrorCode error);

#endif
