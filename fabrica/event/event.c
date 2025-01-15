#include "fabrica/event/event.h"
#include "fabrica/debug.h"
#include "fabrica/error.h"
#include "fabrica/input/keyboard.h"
#include "fabrica/input/mouse.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static bool s_event_init = false;
static fabrica_Event *s_event_queue;
static int s_event_queue_len;
static int s_event_queue_cap;

void fabrica_event_init() {
    s_event_init = true;
    s_event_queue_len = 0;
    s_event_queue_cap = 32;
    s_event_queue = malloc(sizeof(fabrica_Event) * s_event_queue_cap);

    if (s_event_queue == NULL) {
        fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
    }
}

void fabrica_event_terminate() {
    assert(s_event_init && "The event subsystem must be initialized");

    free(s_event_queue);
    s_event_init = false;
}

void fabrica_push_event(fabrica_Event *event) {
    assert(s_event_init && "The event subsystem must be initialized");
    assert(event != NULL);

    int new_len = s_event_queue_len + 1;

    if (new_len >= s_event_queue_cap) {
        s_event_queue_cap *= 2;
        s_event_queue =
            realloc(s_event_queue, s_event_queue_cap * sizeof(*s_event_queue));

        if (s_event_queue == NULL) {
            fabrica_exit(fabrica_ErrorCode_MEMORY_ALLOCATION);
        }
    }

    s_event_queue[s_event_queue_len] = *event;
    s_event_queue_len++;
}

bool fabrica_poll_event(fabrica_Event *event) {
    assert(s_event_init && "The event subsystem must be initialized");
    assert(event != NULL);

    if (s_event_queue_len == 0) {
        event->type = fabrica_EventType_NONE;
        return false;
    }

    *event = s_event_queue[0];

    for (int i = 1; i < s_event_queue_len; ++i) {
        s_event_queue[i - 1] = s_event_queue[i];
    }

    s_event_queue_len--;

    return true;
}

void fabrica_event_handle_events(fabrica_Game *game) {
    fabrica_Event event = {0};

    while (fabrica_poll_event(&event)) {
        switch (event.type) {
        case fabrica_EventType_KEY:
            fabrica_keyboard_handle_key_event(&event);
            break;

        case fabrica_EventType_WINDOW_CLOSE:
            game->is_running = false;
            break;

        case fabrica_EventType_CURSOR_POS: {
            fabrica_mouse_handle_cursor_pos_event(&event);
            break;
        }

        case fabrica_EventType_MOUSE_BUTTON:
            fabrica_mouse_handle_mouse_button_event(&event);
            break;

        default:
            break;
        }
    }
}
