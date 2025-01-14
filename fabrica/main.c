#include "fabrica/error.h"
#include "fabrica/event/event.h"
#include "fabrica/input/key.h"
#include "fabrica/input/keyboard.h"
#include "fabrica/input/mouse.h"
#include "fabrica/input/mouse_button.h"
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

#include <stb/stb_image.h>

#include <assert.h>
#include <stdbool.h>

static int s_is_running = 1;

static double s_previous_cursor_x = -1;
static double s_previous_cursor_y = -1;
static double s_block_breaking_cooldown = 0.0;
static double s_block_placinplacing_cooldown = 0.0;

bool s_left_mouse_pressed = false;
bool s_right_mouse_pressed = false;

static fabrica_Camera s_camera;

int main() {
    fabrica_error_init();
    fabrica_event_init();

    if (!fabrica_renderer_init()) {
        fabrica_error_print_and_clear();
        return 1;
    }

    int width, height;
    fabrica_renderer_get_window_dimensions(&width, &height);

    s_previous_cursor_x = width / 2.0;
    s_previous_cursor_y = height / 2.0;

    fabrica_mouse_set_cursor_pos(s_previous_cursor_x, s_previous_cursor_y);

    fabrica_TextureAtlas atlas;
    fabrica_blocks_init(&atlas);

    fabrica_camera_init(&s_camera, (fabrica_Vec3F){0.0f, 0.0f, -1.0f},
                        (fabrica_Vec3F){0.0f, 0.0f, 1.0f});

    fabrica_World world;
    fabrica_world_init(&world, &atlas);

    fabrica_ShaderProgram *shader_program =
        fabrica_shaders_get(fabrica_ShaderProgramType_TEXTURED);

    double start_time = 0;
    double delta_time = 1 / 60.0;

    while (s_is_running) {
        // Delta Time
        if (start_time == 0) {
            start_time = glfwGetTime();
        } else {
            double current_time = glfwGetTime();
            delta_time = current_time - start_time;
            start_time = current_time;
        }

        // Event handling
        fabrica_Event event = {0};

        while (fabrica_poll_event(&event)) {
            switch (event.type) {
            case fabrica_EventType_KEY:
                fabrica_keyboard_handle_key_event(&event);
                break;

            case fabrica_EventType_WINDOW_CLOSE:
                s_is_running = false;
                break;

            case fabrica_EventType_CURSOR_POS: {
                fabrica_mouse_handle_cursor_pos_event(&event);
                break;
            }

            case fabrica_EventType_MOUSE_BUTTON:
                fabrica_mouse_handle_mouse_button_event(&event);
                break;

            default:
                break;
            }
        }

        // Update
        double cursor_x, cursor_y;
        fabrica_mouse_get_cursor_pos(&cursor_x, &cursor_y);

        float x_offset = (cursor_x - s_previous_cursor_x) *
                         FABRICA_MOUSE_SENSIBILITY * delta_time;
        float y_offset = (cursor_y - s_previous_cursor_y) *
                         FABRICA_MOUSE_SENSIBILITY * delta_time;

        s_previous_cursor_x = cursor_x;
        s_previous_cursor_y = cursor_y;

        fabrica_camera_rotate(&s_camera, x_offset, y_offset);
        fabrica_camera_recalculate_vectors(&s_camera);

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_ESCAPE)) {
            s_is_running = false;
        }

        fabrica_CameraMoveDir movedir;

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_W)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_FORWARD,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_S)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_BACKWARD,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_D)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_RIGHT,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_A)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_LEFT,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_SPACE)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_UP,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        if (fabrica_keyboard_is_key_pressed(fabrica_Key_LEFT_SHIFT)) {
            fabrica_camera_move(&s_camera, fabrica_CameraMoveDir_DOWN,
                                FABRICA_PLAYER_SPEED * delta_time);
        }

        fabrica_RaycastHit hit = {0};
        fabrica_Vec3F dir = {0};

        fabrica_vec3f_normalize(&s_camera.front, &dir);

        // TODO: Check if the max ray length is correct
        fabrica_raycast(&world, &s_camera.pos, &dir, &hit,
                        FABRICA_PLAYER_INTERACTION_MAX_LENGTH);

        // Block breaking
        if (fabrica_mouse_is_button_pressed(fabrica_MouseButton_LEFT) &&
            hit.hit && s_block_breaking_cooldown <= 0) {
            fabrica_Block *block =
                fabrica_world_get_block(&world, &hit.block_pos);

            if (block != NULL) {
                const fabrica_BlockTypeInfo *info =
                    fabrica_block_get_type_info(block->type);

                if (info != NULL) {
                    block->type = fabrica_BlockType_AIR;
                    fabrica_world_mark_chunk_dirty_by_block_pos(&world,
                                                                &hit.block_pos);
                    s_block_breaking_cooldown = FABRICA_BLOCK_BREAKING_COOLDOWN;
                }
            }
        }

        if (s_block_breaking_cooldown >= 0) {
            s_block_breaking_cooldown -= delta_time;
        }

        // Block placing
        if (fabrica_mouse_is_button_pressed(fabrica_MouseButton_RIGHT) &&
            hit.hit && s_block_breaking_cooldown <= 0) {
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
                s_block_breaking_cooldown = FABRICA_BLOCK_PLACING_COOLDOWN;
            }
        }

        if (s_block_breaking_cooldown >= 0) {
            s_block_breaking_cooldown -= delta_time;
        }

        // Render
        fabrica_render(&world, &s_camera, &atlas);
    }

    fabrica_world_destroy(&world);
    fabrica_texture_atlas_destroy(&atlas);
    fabrica_renderer_terminate();
    fabrica_event_terminate();
    fabrica_error_terminate();

    return 0;
}
