#ifndef KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_RENDERER_H
#define KASOUZA_FABRICA_CELESTE_FABRICA_RENDERER_RENDERER_H

#include "fabrica/renderer/camera.h"
#include "fabrica/world/world.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>

bool fabrica_renderer_init(const fabrica_Allocator *allocator);
void fabrica_render(const fabrica_World *world, const fabrica_Camera *camera, const fabrica_TextureAtlas *atlas);

GLFWwindow *fabrica_renderer_get_window();

#endif
