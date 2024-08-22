#include "fabrica/file_io/read_file.h"
#include "fabrica/debug.h"
#include "fabrica/error.h"
#include "fabrica/memory/allocator.h"

#include <alloca.h>
#include <errno.h>
#include <stdio.h>

char *fabrica_read_file_string(const char *filename, const fabrica_Allocator *allocator) {
    assert(filename != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) == -1) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno);
        return NULL;
    }

    long length = ftell(file);
    if (length == -1) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno);
        return NULL;
    }

    char *content = allocator->malloc(length + 1);
    if (!content) {
        fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
    }

    if (fseek(file, 0, SEEK_SET) == -1) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno);
        return NULL;
    }

    fread(content, 1, length, file);

    if (ferror(file) != 0) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno);

        fclose(file);
        allocator->free(content);
        return NULL;
    }

    content[length] = '\0';

    return content;
}
