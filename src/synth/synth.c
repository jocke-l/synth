#include "synth.h"

#include <audio/audio.h>
#include <math.h>
#include <stdlib.h>

static double square_oscillator(double time, double frequency, double amplitude, int num_harmonics);
static double sine_oscillator(double time, double frequency, double amplitude);
static double sawtooth_oscillator(double time, double frequency, double amplitude, int num_harmonics);
static double triangle_oscillator(double time, double frequency, double amplitude, int num_harmonics);
static double white_noise(double amplitude);

double synth_callback(double time, const void* context) {
    const Synth *synth = context;
    return square_oscillator(
        (
            time + sine_oscillator(
                time + sine_oscillator(time, 0.05, 1),
                55,
                0.015
            )
        ),
        110,
        synth->volume,
        20
    );
}

static double sine_oscillator(double time, double frequency, double amplitude) {
    return audio_sin_turns(frequency * time) * amplitude;
}

static double square_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double square = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        double k = 2.0 * i + 1.0;
        square += 1.0 / k * sine_oscillator(time, k * frequency, amplitude);
    }

    return square;
}

[[maybe_unused]]
static double sawtooth_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double sawtooth = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        double k = i + 1.0;
        sawtooth += sine_oscillator(time, k * frequency, amplitude) / k;
    }

    return sawtooth;
}

[[maybe_unused]]
static double triangle_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
) {
    double triangle = 0.0;
    for (int i = 0; i < num_harmonics; i++) {
        double k = 2.0 * i + 1.0;
        triangle += (
            (pow(-1.0, (k - 1.0) / 2.0) / pow(k, 2.0)) *
            sine_oscillator(time, k * frequency, amplitude)
        );
    }

    return triangle;
}

[[maybe_unused]]
static double white_noise(double amplitude) {
    return (((double)rand()) / (double)RAND_MAX) * amplitude;
}

