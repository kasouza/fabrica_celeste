#include "fabrica/error.h"
#include "fabrica/debug.h"
#include "fabrica/memory/allocator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const fabrica_Allocator *s_allocator;

static fabrica_Error *s_errors;
static int s_errors_len;
static int s_errors_cap;

static const fabrica_Error **s_error_stack;
static int s_error_stack_len;
static int s_error_stack_cap;

void fabrica_error_init(const fabrica_Allocator *allocator) {
    s_allocator = allocator;

    s_errors_cap = 32;
    s_errors_len = 0;
    s_errors = s_allocator->malloc(sizeof(fabrica_Error) * s_errors_cap);

    if (!s_errors) {
        fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
    }

    s_error_stack_cap = 32;
    s_error_stack_len = 0;
    s_error_stack =
        s_allocator->malloc(sizeof(fabrica_Error *) * s_error_stack_cap);

    if (!s_error_stack) {
        fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
    }
}

void fabrica_error_terminate() {
    for (int i = 0; i < s_errors_len; i++) {
        if (s_errors[i].message) {
            free(s_errors[i].message);
        }
    }

    free(s_errors);
    s_errors = NULL;

    free(s_error_stack);
}

void fabrica_error_push(fabrica_ErrorCode code) {
    fabrica_error_push_errno(code, 0, NULL);
}

void fabrica_error_push_message(fabrica_ErrorCode code, const char *message) {
    fabrica_error_push_errno(code, 0, message);
}

void fabrica_error_push_errno(fabrica_ErrorCode code, int errno,
                              const char *message) {
    assert(s_errors != NULL && "Errro module is not initialized");

    // Grow arrays
    if (s_errors_len >= s_errors_cap) {
        int new_cap = s_errors_cap * 2;
        s_errors =
            s_allocator->realloc(s_errors, new_cap * sizeof(fabrica_Error));
        if (!s_errors) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }

        s_errors_cap = new_cap;
    }

    if (s_error_stack_len >= s_error_stack_cap) {
        int new_cap = s_error_stack_cap * 2;
        s_error_stack = s_allocator->realloc(s_error_stack,
                                             new_cap * sizeof(fabrica_Error *));
        if (!s_error_stack) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }

        s_error_stack_cap = new_cap;
    }

    // Push error to s_errors
    int idx = s_errors_len;
    s_errors_len++;

    s_errors[idx].code = code;
    s_errors[idx].errno = errno;
    s_errors[idx].message = NULL;

    if (message) {
        int size = strlen(message) + 1;

        s_errors[idx].message = s_allocator->malloc(size * sizeof(char));
        if (!s_errors[idx].message) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }

        memcpy(s_errors[idx].message, message, size);
    }

    // Push error to s_error_stack
    int stack_idx = s_error_stack_len;
    s_error_stack_len++;

    s_error_stack[stack_idx] = &s_errors[idx];
}

const fabrica_Error *fabrica_error_pop() {
    if (s_error_stack_len <= 0) {
        return NULL;
    }

    s_error_stack_len--;

    return s_error_stack[s_error_stack_len];
}

void fabrica_error_print_and_clear() {
    const fabrica_Error *error = fabrica_error_pop();
    while (error) {
        char *message = "<no message>";
        if (error->message) {
            message = error->message;
        }

        fprintf(stderr, "Error: %d - %s\n", error->code, message);

        if (error->errno) {
            fprintf(stderr, "Errno: %d - %s\n", error->errno,
                    strerror(error->errno));
        }

        fprintf(stderr, "\n");

        error = fabrica_error_pop();
    }
}

void fabrica_exit(fabrica_ErrorCode error) { exit(error); }
