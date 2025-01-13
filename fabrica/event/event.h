#ifndef KASOUZA_FABRICA_FABRICA_EVENT_EVENT_H
#define KASOUZA_FABRICA_FABRICA_EVENT_EVENT_H

#include "fabrica/input/action.h"
#include "fabrica/input/key.h"

#include <stdbool.h>

typedef struct GLFWwindow GLFWwindow;

/** Event types */
typedef enum {
    fabrica_EventType_NONE = 0,
    fabrica_EventType_WINDOW_CLOSE,
    fabrica_EventType_MOUSE_BUTTON,
    fabrica_EventType_KEY,
    fabrica_EventType_COUNT
} fabrica_EventType;

/** Event structs */
typedef struct {
    fabrica_EventType type;
    GLFWwindow *window;
} fabrica_WindowCloseEvent;

typedef struct {
    fabrica_EventType type;
    int button;
    int action;
    int mods;
} fabrica_MouseButtonEvent;

typedef struct {
    fabrica_EventType type;
    fabrica_Key key;
    fabrica_Action action;
} fabrica_KeyEvent;

/** Union of all event types */
typedef union {
    fabrica_EventType type;
    fabrica_WindowCloseEvent window_close;
    fabrica_MouseButtonEvent mouse_button;
    fabrica_KeyEvent key;
} fabrica_Event;

void fabrica_event_init();
void fabrica_event_terminate();

void fabrica_push_event(fabrica_Event *event);
bool fabrica_poll_event(fabrica_Event *event);

#endif
