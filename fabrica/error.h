#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_ERROR_H

#include "fabrica/memory/allocator.h"
typedef enum {
    fabrica_ErrorCode_OK = 0,
    fabrica_ErrorCode_MEMORY_ALLOCATION = 1,
    fabrica_ErrorCode_READ_FILE = 2,
    fabrica_ErrorCode_RENDERER_SHADER_COMPILATION = 3,
    fabrica_ErrorCode_RENDERER_SHADER_PROGRAM_LINKING = 4
} fabrica_ErrorCode;

typedef struct {
    fabrica_ErrorCode code;
    int errno;
    char *message;
} fabrica_Error;

void fabrica_error_init(const fabrica_Allocator *allocator);
void fabrica_error_terminate();

void fabrica_error_push(fabrica_ErrorCode code);

/**
 * Pushes an error to the error stack
 * The `message` param will be copied and have it's memory managed
 * by the `error` module
 */
void fabrica_error_push_message(fabrica_ErrorCode code, const char *message);

/**
 * Pushes an error to the error stack
 * The `message` param will be copied and have it's memory managed
 * by the `error` module
 */
void fabrica_error_push_errno(fabrica_ErrorCode code, int errno,
                              const char *message);

const fabrica_Error *fabrica_error_pop();

void fabrica_error_print_and_clear();

void fabrica_exit(fabrica_ErrorCode error);

#endif
