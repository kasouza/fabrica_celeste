#ifndef KASOUZA_FABRICA_GLFW_H
#define KASOUZA_FABRICA_GLFW_H

#include "fabrica/renderer/gl.h"
#include <stdbool.h>

bool fabrica_glfw_init();
void fabrica_glfw_terminate();
GLFWwindow *fabrica_glfw_get_window();
void fabrica_glfw_get_window_dimensions(int *width, int *height);

#endif
