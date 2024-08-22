#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_FILE_IO_READ_FILE_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_FILE_IO_READ_FILE_H

#include "fabrica/memory/allocator.h"

/**
 * Returns a null terminated string or NULL on failure
 * If the function fails, an error will be added to the error stack,
 * which can be retrived callindg `fabrica_error_pop`
 */
char *fabrica_read_file_string(const char *filename,
                               const fabrica_Allocator *allocator);

#endif
