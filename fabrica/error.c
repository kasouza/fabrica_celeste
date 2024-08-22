#include "fabrica/error.h"
#include "fabrica/debug.h"
#include "fabrica/memory/allocator.h"

#include <stdlib.h>
#include <string.h>

static const fabrica_Allocator *s_allocator;

static fabrica_Error *s_errors;
static int s_errors_len;
static int s_errors_cap;

void fabrica_error_init(const fabrica_Allocator *allocator) {
    s_allocator = allocator;

    s_errors_cap = 32;
    s_errors_len = 0;
    s_errors = s_allocator->malloc(sizeof(fabrica_Error) * s_errors_cap);
}

void fabrica_error_terminate() {
    free(s_errors);
    s_errors = NULL;
}

void fabrica_error_push(fabrica_ErrorCode code) {
    fabrica_error_push_errno(code, 0);
}

void fabrica_error_push_errno(fabrica_ErrorCode code, int errno) {
    assert(s_errors != NULL && "Errro module is not initialized");

    if (s_errors_len >= s_errors_cap) {
        int new_cap = s_errors_cap * 2;
        s_errors =
            s_allocator->realloc(s_errors, new_cap * sizeof(fabrica_Error));
        if (!s_errors) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }

        s_errors_cap = new_cap;
    }

    int idx = s_errors_len;
    s_errors_len++;

    s_errors[idx].code = code;
    s_errors[idx].errno = errno;
}

const fabrica_Error *fabrica_error_pop() {
    if (s_errors_len <= 0) {
        return NULL;
    }

    int idx = s_errors_len - 1;
    s_errors_len--;

    return &s_errors[idx];
}

void fabrica_exit(fabrica_ErrorCode error) { exit(error); }
