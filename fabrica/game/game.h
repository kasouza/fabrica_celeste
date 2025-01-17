#ifndef KASOUZA_FABRICA_GAME_GAME_H
#define KASOUZA_FABRICA_GAME_GAME_H

#include "fabrica/renderer/camera.h"
#include "fabrica/renderer/texture_atlas.h"
#include "fabrica/world/world.h"

#include <stdbool.h>

typedef struct {
    bool is_running;
    fabrica_Camera camera;
    fabrica_World world;
    fabrica_TextureAtlas atlas;

    double previous_cursor_x;
    double previous_cursor_y;

    double block_breaking_cooldown;
    double block_placinplacing_cooldown;

    double dt_start_time;
    double dt;
} fabrica_Game;

void fabrica_game_init(fabrica_Game *game);
void fabrica_game_terminate(fabrica_Game *game);

void fabrica_game_run(fabrica_Game *game);

#endif
