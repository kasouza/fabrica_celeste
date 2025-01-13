#include "fabrica/event/event.h"

void handle_key_event(GLFWwindow *window, int key, int scancode, int action,
                      int mods) {
    fabrica_Event event;
    event.type = fabrica_EventType_KEY;
    event.key.key = fabrica_key_from_glfw(key);
    event.key.action = fabrica_action_from_glfw(action);

    fabrica_push_event(&event);
}
