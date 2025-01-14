#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_RENDERER_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_RENDERER_H

#include "fabrica/renderer/camera.h"
#include "fabrica/world/world.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>

bool fabrica_renderer_init();
void fabrica_render(fabrica_World *world, const fabrica_Camera *camera, const fabrica_TextureAtlas *atlas);
void fabrica_renderer_terminate();

GLFWwindow *fabrica_renderer_get_window();
void fabrica_renderer_get_window_dimensions(int *width, int *height);
void fabrica_renderer_set_cursor_pos(double x, double y);

#endif
