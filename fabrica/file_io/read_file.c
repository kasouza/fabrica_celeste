#include <alloca.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>

#include "fabrica/debug.h"
#include "fabrica/error.h"

char *fabrica_read_file_string(const char *filename) {
    assert(filename != NULL);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno, NULL);
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) == -1) {
        fclose(file);
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno, NULL);
        return NULL;
    }

    long length = ftell(file);
    if (length == -1) {
        fclose(file);
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno, NULL);
        return NULL;
    }

    char *content = malloc(length + 1);
    if (!content) {
        fclose(file);
        fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
    }

    if (fseek(file, 0, SEEK_SET) == -1) {
        free(content);
        fclose(file);

        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno, NULL);
        return NULL;
    }

    fread(content, 1, length, file);

    if (ferror(file) != 0) {
        free(content);
        fclose(file);
        fabrica_error_push_errno(fabrica_ErrorCode_READ_FILE, errno, NULL);

        return NULL;
    }

    fclose(file);

    content[length] = '\0';

    return content;
}
