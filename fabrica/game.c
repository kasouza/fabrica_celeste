#include "fabrica/game.h"
#include "fabrica/input/keyboard.h"
#include "fabrica/input/mouse.h"
#include "fabrica/renderer/renderer.h"
#include "fabrica/utils/constants.h"
#include "fabrica/world/raycast.h"
#include "fabrica/world/world.h"
#include <stdio.h>

void tick(fabrica_Game *game);
void render(fabrica_Game *game);

void fabrica_game_init(fabrica_Game *game) {
    assert(game != NULL);

    int width, height;
    fabrica_renderer_get_window_dimensions(&width, &height);

    game->previous_cursor_x = width / 2.0;
    game->previous_cursor_y = height / 2.0;

    fabrica_mouse_set_cursor_pos(game->previous_cursor_x,
                                 game->previous_cursor_y);

    fabrica_blocks_init(&game->atlas);

    fabrica_camera_init(&game->camera, (fabrica_Vec3F){0.0f, 0.0f, 0.0f},
                        (fabrica_Vec3F){0.0f, 0.0f, 1.0f});

    fabrica_world_init(&game->world);

    game->dt = 1 / 60.0;
    game->dt_start_time = 0;
}

void fabrica_game_terminate(fabrica_Game *game) {
    assert(game != NULL);
    fabrica_world_destroy(&game->world);
    fabrica_texture_atlas_destroy(&game->atlas);
}

void fabrica_game_run(fabrica_Game *game) {
    while (game->is_running) {
        // Delta Time
        if (game->dt_start_time == 0) {
            game->dt_start_time = glfwGetTime();
        } else {
            double current_time = glfwGetTime();
            game->dt = current_time - game->dt_start_time;
            game->dt_start_time = current_time;
        }

        fabrica_event_handle_events(game);
        tick(game);
        fabrica_renderer_render(game);
    }
}

void tick(fabrica_Game *game) {
    double cursor_x, cursor_y;
    fabrica_mouse_get_cursor_pos(&cursor_x, &cursor_y);

    float x_offset = (cursor_x - game->previous_cursor_x) *
                     FABRICA_MOUSE_SENSIBILITY * game->dt;
    float y_offset = (cursor_y - game->previous_cursor_y) *
                     FABRICA_MOUSE_SENSIBILITY * game->dt;

    game->previous_cursor_x = cursor_x;
    game->previous_cursor_y = cursor_y;

    fabrica_camera_rotate(&game->camera, x_offset, y_offset);
    fabrica_camera_recalculate_vectors(&game->camera);

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_ESCAPE)) {
        game->is_running = false;
    }

    fabrica_CameraMoveDir movedir;

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_W)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_FORWARD,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_S)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_BACKWARD,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_D)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_RIGHT,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_A)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_LEFT,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_SPACE)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_UP,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    if (fabrica_keyboard_is_key_pressed(fabrica_Key_LEFT_SHIFT)) {
        fabrica_camera_move(&game->camera, fabrica_CameraMoveDir_DOWN,
                            FABRICA_PLAYER_SPEED * game->dt);
    }

    fabrica_RaycastHit hit = {0};
    fabrica_Vec3F dir = {0};

    fabrica_vec3f_normalize(&game->camera.front, &dir);

    // TODO: Check if the max ray length is correct
    fabrica_raycast(&game->world, &game->camera.pos, &dir, &hit,
                    FABRICA_PLAYER_INTERACTION_MAX_LENGTH);

    // Block breaking
    if (fabrica_mouse_is_button_pressed(fabrica_MouseButton_LEFT) && hit.hit &&
        game->block_breaking_cooldown <= 0) {
        fabrica_Block *block =
            fabrica_world_get_block(&game->world, &hit.block_pos);

        if (block != NULL) {
            const fabrica_BlockTypeInfo *info =
                fabrica_block_get_type_info(block->type);

            if (info != NULL) {
                block->type = fabrica_BlockType_AIR;
                fabrica_world_mark_chunk_dirty_by_block_pos(&game->world,
                                                            &hit.block_pos);
                game->block_breaking_cooldown = FABRICA_BLOCK_BREAKING_COOLDOWN;
            }
        }
    }

    if (game->block_breaking_cooldown >= 0) {
        game->block_breaking_cooldown -= game->dt;
    }

    // Block placing
    if (fabrica_mouse_is_button_pressed(fabrica_MouseButton_RIGHT) && hit.hit &&
        game->block_breaking_cooldown <= 0) {
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
            fabrica_world_get_block(&game->world, &new_block_pos);

        if (block != NULL && block->type == fabrica_BlockType_AIR) {
            block->type = fabrica_BlockType_DIRT;
            fabrica_world_mark_chunk_dirty_by_block_pos(&game->world,
                                                        &new_block_pos);
            game->block_breaking_cooldown = FABRICA_BLOCK_PLACING_COOLDOWN;
        }
    }

    if (game->block_breaking_cooldown >= 0) {
        game->block_breaking_cooldown -= game->dt;
    }

    fabrica_world_unload_far_chunks(&game->world, &game->camera.pos, 1);
}
