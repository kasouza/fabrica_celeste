#include "fabrica/input/mouse.h"
#include "fabrica/debug.h"
#include "fabrica/input/mouse_button.h"
#include "fabrica/renderer/renderer.h"

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

void fabrica_mouse_handle_cursor_pos_event(const fabrica_Event *event) {
    s_cursor_x_offset = event->cursor_pos.x - s_cursor_x;
    s_cursor_y_offset = event->cursor_pos.y - s_cursor_y;
    s_cursor_x = event->cursor_pos.x;
    s_cursor_y = event->cursor_pos.y;
}

void fabrica_mouse_get_cursor_pos(double *x, double *y) {
    assert(x != NULL);
    assert(y != NULL);

    *x = s_cursor_x;
    *y = s_cursor_y;
}

void fabrica_mouse_set_cursor_pos(double x, double y) {
    s_cursor_x = x;
    s_cursor_y = y;
    fabrica_renderer_set_cursor_pos(x, y);
}
