#include "fabrica/input/mouse_button.h"
#include "fabrica/debug.h"
#include "fabrica/renderer/gl.h"

static int s_glfw_mouse_button_to_fabrica_mouse_button[] = {
    [GLFW_MOUSE_BUTTON_1] = fabrica_MouseButton_1,
    [GLFW_MOUSE_BUTTON_2] = fabrica_MouseButton_2,

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Winitializer-overrides"
    [GLFW_MOUSE_BUTTON_LEFT] = fabrica_MouseButton_LEFT,
    [GLFW_MOUSE_BUTTON_RIGHT] = fabrica_MouseButton_RIGHT,
#pragma clang diagnostic pop

    [GLFW_MOUSE_BUTTON_3] = fabrica_MouseButton_3,
    [GLFW_MOUSE_BUTTON_4] = fabrica_MouseButton_4,
    [GLFW_MOUSE_BUTTON_5] = fabrica_MouseButton_5,
    [GLFW_MOUSE_BUTTON_6] = fabrica_MouseButton_6,
    [GLFW_MOUSE_BUTTON_7] = fabrica_MouseButton_7,
    [GLFW_MOUSE_BUTTON_8] = fabrica_MouseButton_8,
};

fabrica_MouseButton fabrica_mouse_button_from_glfw(int glfw_button) {
    assert(glfw_button >= 0 && glfw_button <= GLFW_MOUSE_BUTTON_LAST);
    return s_glfw_mouse_button_to_fabrica_mouse_button[glfw_button];
}
