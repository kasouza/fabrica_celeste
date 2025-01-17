#include "fabrica/input/mouse.h"
#include "fabrica/debug.h"
#include "fabrica/input/mouse_button.h"

#include <stddef.h>

static bool s_keys[fabrica_MouseButton_COUNT] = {0};

static double s_cursor_x = 0;
static double s_cursor_y = 0;

static double s_cursor_x_offset = 0;
static double s_cursor_y_offset = 0;

void fabrica_mouse_handle_mouse_button_event(const fabrica_Event *event) {
    if (event->mouse_button.action == fabrica_Action_PRESS) {
        s_keys[event->mouse_button.button] = true;
    } else if (event->mouse_button.action == fabrica_Action_RELEASE) {
        s_keys[event->mouse_button.button] = false;
    }
}

bool fabrica_mouse_is_button_pressed(fabrica_MouseButton key) {
    assert(key >= 0 && key < fabrica_MouseButton_COUNT);
    return s_keys[key];
}
