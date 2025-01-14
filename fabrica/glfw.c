#include "fabrica/error.h"
#include "fabrica/event/event.h"
#include "fabrica/input/mouse_button.h"
#include "fabrica/renderer/gl.h"

#include <GLFW/glfw3.h>
#include <stdio.h>

static GLFWwindow *s_window = NULL;
static int s_window_width = 800;
static int s_window_height = 600;

void fabrica_glfw_handle_key_event(GLFWwindow *window, int key, int scancode,
                                   int action, int mods) {
    fabrica_Event event;
    event.type = fabrica_EventType_KEY;
    event.key.key = fabrica_key_from_glfw(key);
    event.key.action = fabrica_action_from_glfw(action);

    fabrica_push_event(&event);
}

void fabrica_glfw_handle_window_close_event(GLFWwindow *window) {
    fabrica_Event event;
    event.type = fabrica_EventType_WINDOW_CLOSE;
    fabrica_push_event(&event);
}

void fabrica_glfw_handle_cursor_pos_event(GLFWwindow *window, double x,
                                          double y) {
    fabrica_Event event;
    event.type = fabrica_EventType_CURSOR_POS;
    event.cursor_pos.x = x;
    event.cursor_pos.y = y;

    fabrica_push_event(&event);
}

void fabrica_glfw_handle_mouse_button_event(GLFWwindow *window, int button,
                                            int action, int mods) {
    fabrica_Event event;
    event.mouse_button.type = fabrica_EventType_MOUSE_BUTTON;
    event.mouse_button.button = fabrica_mouse_button_from_glfw(button);
    event.mouse_button.action = fabrica_action_from_glfw(action);

    fabrica_push_event(&event);
}

void handle_window_resize_event(GLFWwindow *window, int width, int height) {
    s_window_width = width;
    s_window_height = height;

    glViewport(0, 0, width, height);
}

bool fabrica_glfw_init() {
    if (!glfwInit()) {
        fabrica_error_push_message(fabrica_ErrorCode_GLFW_INITIALIZATION,
                                   "Failed to initialize GLFW\n");
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    s_window =
        glfwCreateWindow(s_window_width, s_window_height, "Mine", NULL, NULL);
    if (!s_window) {
        fabrica_error_push_message(fabrica_ErrorCode_GLFW_INITIALIZATION,
                                   "Failed to create window\n");
        return false;
    }

    glfwMakeContextCurrent(s_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fabrica_error_push_message(fabrica_ErrorCode_GLFW_INITIALIZATION,
                                   "Failed to initialize GLAD\n");
        return false;
    }

    glViewport(0, 0, s_window_width, s_window_height);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);

    glfwSetWindowSizeCallback(s_window, handle_window_resize_event);

    glfwSetInputMode(s_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(s_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetKeyCallback(s_window, fabrica_glfw_handle_key_event);
    glfwSetWindowCloseCallback(s_window,
                               fabrica_glfw_handle_window_close_event);
    glfwSetCursorPosCallback(s_window, fabrica_glfw_handle_cursor_pos_event);
    glfwSetMouseButtonCallback(s_window,
                               fabrica_glfw_handle_mouse_button_event);

    return true;
}

void fabrica_glfw_terminate() {
    if (s_window != NULL) {
        glfwDestroyWindow(s_window);
        s_window = NULL;
    }

    glfwTerminate();
}

GLFWwindow *fabrica_glfw_get_window() { return s_window; }

void fabrica_glfw_get_window_dimensions(int *width, int *height) {
    if (width != NULL) {
        *width = s_window_width;
    }

    if (height != NULL) {
        *height = s_window_height;
    }
}
