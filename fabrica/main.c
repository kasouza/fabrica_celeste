#include "fabrica/error.h"
#include "fabrica/event/event.h"
#include "fabrica/event/glfw.h"
#include "fabrica/input/key.h"
#include "fabrica/input/keyboard.h"
#include "fabrica/math/vec3f.h"
#include "fabrica/math/vec3i.h"
#include "fabrica/renderer/camera.h"
#include "fabrica/renderer/renderer.h"
#include "fabrica/renderer/shaders.h"
#include "fabrica/renderer/texture_atlas.h"
#include "fabrica/utils/constants.h"
#include "fabrica/world/block.h"
#include "fabrica/world/raycast.h"
#include "fabrica/world/world.h"

#include <GLFW/glfw3.h>
#include <alloca.h>
#include <stb/stb_image.h>

#include <assert.h>
#include <stdbool.h>

static int s_is_running = 1;

static double s_cursor_pos_x;
static double s_cursor_pos_y;

bool s_left_mouse_pressed = false;
bool s_right_mouse_pressed = false;

static fabrica_Camera s_camera;

void handle_window_close_event(GLFWwindow *window) { s_is_running = 0; }

void handle_cursor_pos_event(GLFWwindow *window, double x, double y) {
    float x_offset = (x - s_cursor_pos_x) * 0.01;
    float y_offset = (y - s_cursor_pos_y) * 0.01;

    s_cursor_pos_x = x;
    s_cursor_pos_y = y;

    fabrica_camera_rotate(&s_camera, x_offset, y_offset);
    fabrica_camera_recalculate_vectors(&s_camera);
}

void handle_mouse_button_event(GLFWwindow *window, int button, int action,
                               int mods) {
    fabrica_Event event;
    event.mouse_button.type = fabrica_EventType_MOUSE_BUTTON;
    event.mouse_button.button = button;
    event.mouse_button.action = action;
    event.mouse_button.mods = mods;

    fabrica_push_event(&event);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        s_left_mouse_pressed = action == GLFW_PRESS;
    }

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        s_right_mouse_pressed = action == GLFW_PRESS;
    }
}

void init_events(GLFWwindow *window) {
    glfwSetKeyCallback(window, handle_key_event);
    glfwSetWindowCloseCallback(window, handle_window_close_event);
    glfwSetCursorPosCallback(window, handle_cursor_pos_event);
    glfwSetMouseButtonCallback(window, handle_mouse_button_event);
}

int main() {
    fabrica_error_init();

    if (!fabrica_renderer_init()) {
        fabrica_error_print_and_clear();
        return 1;
    }

    fabrica_event_init();

    fabrica_TextureAtlas atlas;
    fabrica_blocks_init(&atlas);

    init_events(fabrica_renderer_get_window());

    fabrica_camera_init(&s_camera, (fabrica_Vec3F){0.0f, 0.0f, -1.0f},
                        (fabrica_Vec3F){0.0f, 0.0f, 1.0f});

    glfwGetCursorPos(fabrica_renderer_get_window(), &s_cursor_pos_x,
                     &s_cursor_pos_y);

    fabrica_World world;
    fabrica_world_init(&world, &atlas);

    fabrica_ShaderProgram *shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_TEXTURED);

    while (s_is_running) {
        // Event handling
        fabrica_Event event = {0};

        while (fabrica_poll_event(&event)) {
            switch (event.type) {
            case fabrica_EventType_KEY:
                fabrica_keyboard_handle_key_event(&event);
                break;

            default:
                break;
            }
        }

        // Update
        if (fabrica_keyboard_is_key_pressed(fabrica_Key_ESCAPE)) {
            s_is_running = false;
        }

        fabrica_CameraMoveDir movedir;

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_W)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_FORWARD, 1);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_S)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_BACKWARD, 1);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_D)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_RIGHT, 1);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_A)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_LEFT, 1);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_SPACE)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_UP, 1);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_LEFT_SHIFT)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_DOWN, 1);
        }

        fabrica_RaycastHit hit = {0};
        fabrica_Vec3F dir = {0};

        fabrica_vec3f_normalize(&s_camera.front, &dir);

        // TODO: Check if the max ray length is correct
        fabrica_raycast(&world, &s_camera.pos, &dir, &hit,
                        FABRICA_PLAYER_INTERACTION_MAX_LENGTH);

        if (s_left_mouse_pressed && hit.hit) {
            fabrica_Block *block =
                fabrica_world_get_block(&world, &hit.block_pos);

            if (block != NULL) {
                const fabrica_BlockTypeInfo *info =
                    fabrica_block_get_type_info(block->type);

                if (info != NULL) {
                    block->type = fabrica_BlockType_AIR;
                    fabrica_world_mark_chunk_dirty_by_block_pos(&world,
                                                                &hit.block_pos);
                }
            }
        }

        if (s_right_mouse_pressed && hit.hit) {
            fabrica_Vec3I new_block_pos = hit.block_pos;
            switch (hit.face) {
            case fabrica_Face_BOTTOM:
                new_block_pos.y--;
                break;
            case fabrica_Face_TOP:
                new_block_pos.y++;
                break;
            case fabrica_Face_LEFT:
                new_block_pos.x--;
                break;
            case fabrica_Face_RIGHT:
                new_block_pos.x++;
                break;
            case fabrica_Face_FRONT:
                new_block_pos.z--;
                break;
            case fabrica_Face_BACK:
                new_block_pos.z++;
                break;
            }

            fabrica_Block *block =
                fabrica_world_get_block(&world, &new_block_pos);

            if (block != NULL && block->type == fabrica_BlockType_AIR) {
                block->type = fabrica_BlockType_DIRT;
                fabrica_world_mark_chunk_dirty_by_block_pos(&world,
                                                            &new_block_pos);
            }
        }

        // Render
        fabrica_render(&world, &s_camera, &atlas);
    }

    fabrica_event_terminate();
    glfwTerminate();

    fabrica_world_destroy(&world);
    fabrica_texture_atlas_destroy(&atlas);

    fabrica_error_terminate();

    return 0;
}
