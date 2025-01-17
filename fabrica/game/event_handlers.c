#include "fabrica/game/event_handlers.h"
#include "fabrica/utils/constants.h"

void fabrica_game_handle_cursor_pos_event(const fabrica_Event *event,
                                          fabrica_Game *game) {

    double sensibility =
        FABRICA_MOUSE_SENSIBILITY * FABRICA_MOUSE_SENSIBILITY_FACTOR;
    double x_offset =
        (event->cursor_pos.x - game->previous_cursor_x) * sensibility;
    double y_offset =
        (event->cursor_pos.y - game->previous_cursor_y) * sensibility;
    game->previous_cursor_x = event->cursor_pos.x;
    game->previous_cursor_y = event->cursor_pos.y;

    fabrica_camera_rotate(&game->camera, x_offset, y_offset);
    fabrica_camera_recalculate_vectors(&game->camera);
}

void fabrica_game_handle_window_close_event(const fabrica_Event *event,
                                            fabrica_Game *game) {
    game->is_running = false;
}
