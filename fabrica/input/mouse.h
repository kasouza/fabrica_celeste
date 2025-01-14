#ifndef KASOUZA_FABRICA_FABRICA_INPUT_MOUSE_H
#define KASOUZA_FABRICA_FABRICA_INPUT_MOUSE_H

#include "fabrica/event/event.h"

void fabrica_mouse_handle_mouse_button_event(const fabrica_Event *event);
bool fabrica_mouse_is_button_pressed(fabrica_MouseButton key);

void fabrica_mouse_handle_cursor_pos_event(const fabrica_Event *event);
void fabrica_mouse_get_cursor_pos(double *x, double *y);
void fabrica_mouse_set_cursor_pos(double x, double y);

#endif
