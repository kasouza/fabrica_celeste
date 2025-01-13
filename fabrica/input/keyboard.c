#include "fabrica/input/keyboard.h"
#include "fabrica/event/event.h"
#include "fabrica/input/action.h"

#include <stdbool.h>

static bool s_keys[fabrica_Key_COUNT] = {0};

void fabrica_keyboard_handle_key_event(const fabrica_Event *event) {
    if (event->key.action == fabrica_Action_PRESS) {
        s_keys[event->key.key] = true;
    } else if (event->key.action == fabrica_Action_RELEASE) {
        s_keys[event->key.key] = false;
    }
}

bool fabrica_keyboard_is_key_pressed(fabrica_Key key) { return s_keys[key]; }
