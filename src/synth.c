#include <math.h>
#include <stdlib.h>

#include "synth.h"

#define PI 3.14159265359

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
    return sin(2 * PI * frequency * time) * amplitude;
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

static double white_noise(double amplitude) {
    return (((double)rand()) / (double)RAND_MAX) * amplitude;
}

#include "nuklear.h"
void synth_update_ui(struct nk_context *ctx, Synth *synth) {
    nk_layout_row_dynamic(ctx, 25, 1);
    synth->volume = nk_propertyf(ctx, "Volume", 0, synth->volume, 1.0f, 0.01f, 0.005f);
}
