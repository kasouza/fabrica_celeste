#ifndef KASOUZA_FABRICA_FABRICA_INPUT_KEYBOARD_H
#define KASOUZA_FABRICA_FABRICA_INPUT_KEYBOARD_H

#include "fabrica/event/event.h"

#include <stdbool.h>

void fabrica_keyboard_handle_key_event(const fabrica_Event *event);
bool fabrica_keyboard_is_key_pressed(fabrica_Key key);

#endif
