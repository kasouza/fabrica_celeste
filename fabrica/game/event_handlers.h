#ifndef KASOUZA_FABRICA_GAME_EVENT_HANDLERS_H
#define KASOUZA_FABRICA_GAME_EVENT_HANDLERS_H

#include "fabrica/event/event.h"
#include "fabrica/game/game.h"

void fabrica_game_handle_cursor_pos_event(const fabrica_Event *event,
                                          fabrica_Game *game);

void fabrica_game_handle_window_close_event(const fabrica_Event *event,
                                          fabrica_Game *game);


#endif
