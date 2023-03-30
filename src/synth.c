#include <math.h>
#include <stdlib.h>

#include "synth.h"

#define PI 3.14159265359

double synth_callback(double time, const void* context) {
    return square_oscillator(
        (
            time + sine_oscillator(
                time + sine_oscillator(time, 0.05, 1),
                55,
                0.015
            )
        ),
        110,
        0.3,
        20
    );
}

double sine_oscillator(double time, double frequency, double amplitude) {
    return sin(2 * PI * frequency * time) * amplitude;
}

double square_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double square = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        int k = 2 * i + 1;
        square += 1.0 / k * sine_oscillator(time, k * frequency, amplitude);
    }

    return square;
}

double sawtooth_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double sawtooth = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        int k = i + 1;
        sawtooth += sine_oscillator(time, k * frequency, amplitude) / k;
    }

    return sawtooth;
}

double triangle_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double triangle = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        int k = 2 * i + 1;
        triangle += (
            (pow(-1, (k - 1) / 2.0) / pow(k, 2)) *
            sine_oscillator(time, k * frequency, amplitude)
        );
    }

    return triangle;
}

double white_noise(double amplitude) {
    return rand() / RAND_MAX  * amplitude;
}
