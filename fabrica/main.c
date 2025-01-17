#include "fabrica/error.h"
#include "fabrica/event/event.h"
#include "fabrica/game/game.h"
#include "fabrica/renderer/renderer.h"

#include <stb/stb_image.h>

#include <assert.h>
#include <stdbool.h>

int main() {
    // Initialze subsystems
    fabrica_error_init();
    fabrica_event_init();

    if (!fabrica_renderer_init()) {
        fabrica_error_print_and_clear();
        return 1;
    }

    // Initilize game, enter loop and terminate it
    fabrica_Game game;
    fabrica_game_init(&game);
    fabrica_game_run(&game);

    fabrica_game_terminate(&game);

    // Terminate subsystems
    fabrica_renderer_terminate();
    fabrica_event_terminate();
    fabrica_error_terminate();

    return 0;
}
