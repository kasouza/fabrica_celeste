#ifndef KASOUZA_FABRICA_FABRICA_INPUT_ACTION_H
#define KASOUZA_FABRICA_FABRICA_INPUT_ACTION_H

typedef enum {
    fabrica_Action_RELEASE = 0,
    fabrica_Action_PRESS = 1,
    fabrica_Action_REPEAT = 2
} fabrica_Action;

fabrica_Action fabrica_action_from_glfw(int glfw_action);

#endif
