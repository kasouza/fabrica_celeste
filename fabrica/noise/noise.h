#ifndef KASOUZA_FABRICA_NOISE_NOISE_H
#define KASOUZA_FABRICA_NOISE_NOISE_H

#include <stdint.h>
void fabrica_noise_init(int64_t seed);
void fabrica_noise_terminate();

double fabrica_noise_2d(double x, double y);

#endif
