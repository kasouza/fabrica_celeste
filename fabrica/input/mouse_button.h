#ifndef KASOUZA_FABRICA_FABRICA_INPUT_MOUSE_BUTTON_H
#define KASOUZA_FABRICA_FABRICA_INPUT_MOUSE_BUTTON_H

typedef enum {
    fabrica_MouseButton_NONE,
    fabrica_MouseButton_1,
    fabrica_MouseButton_2,
    fabrica_MouseButton_3,
    fabrica_MouseButton_4,
    fabrica_MouseButton_5,
    fabrica_MouseButton_6,
    fabrica_MouseButton_7,
    fabrica_MouseButton_8,
    fabrica_MouseButton_LEFT,
    fabrica_MouseButton_RIGHT,
    fabrica_MouseButton_MIDDLE,
    fabrica_MouseButton_COUNT,
} fabrica_MouseButton;

fabrica_MouseButton fabrica_mouse_button_from_glfw(int glfw_button);

#endif
