#include <math.h>
#include <stdlib.h>

#include "synth.h"

#define PI 3.14159265359

float synth_callback(double time) {
    return square_oscillator(
        (
            time + sine_oscillator(
//                time,
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

float sine_oscillator(double time, float frequency, float amplitude) {
    return sin(2 * PI * frequency * time) * amplitude;
}

float square_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
) {
    float square = 0.0f;
    for (int i = 0; i < num_harmonics; i++) {
        int k = 2 * i + 1;
        square += 1.0f / k * sine_oscillator(time, k * frequency, amplitude);
    }

    return square;
}

float sawtooth_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
) {
    float sawtooth = 0.0f;
    for (int i = 0; i < num_harmonics; i++) {
        int k = i + 1;
        sawtooth += sine_oscillator(time, k * frequency, amplitude) / k;
    }

    return sawtooth;
}

float triangle_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
) {
    float triangle = 0.0f;
    for (int i = 0; i < num_harmonics; i++) {
        int k = 2 * i + 1;
        triangle += (
            (pow(-1, (k - 1) / 2.0f) / pow(k, 2)) *
            sine_oscillator(time, k * frequency, amplitude)
        );
    }

    return triangle;
}

float white_noise(float amplitude) {
    return 1.0f / (rand() + 1) * amplitude;
}
