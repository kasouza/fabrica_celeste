#include "fabrica/noise/noise.h"
#include "fabrica/error.h"
#include "open_simplex_noise/open_simplex_noise.h"

#include <stddef.h>
#include <stdint.h>

struct osn_context *s_osn_context;

void fabrica_noise_init(int64_t seed) {
    if (open_simplex_noise(seed, &s_osn_context) != 0) {
        fabrica_exit(fabrica_ErrorCode_NOISE_INITIALIZATION);
    }
}

void fabrica_noise_terminate() {
    open_simplex_noise_free(s_osn_context);
    s_osn_context = NULL;
}

double fabrica_noise_2d(double x, double y) {
    return open_simplex_noise2(s_osn_context, x, y);
}
