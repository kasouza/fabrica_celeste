#include "fabrica/input/action.h"
#include "fabrica/renderer/gl.h"

static int s_glfw_actions_to_fabrica_actions[] = {
    [GLFW_RELEASE] = fabrica_Action_RELEASE,
    [GLFW_PRESS] = fabrica_Action_PRESS,
    [GLFW_REPEAT] = fabrica_Action_REPEAT
};

fabrica_Action fabrica_action_from_glfw(int glfw_action) {
    return s_glfw_actions_to_fabrica_actions[glfw_action];
}
