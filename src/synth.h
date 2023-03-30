#pragma once

typedef struct {
    _Atomic double volume;
} Synth;

double synth_callback(double time, const void* synth_context);

struct nk_context;
void synth_update_ui(struct nk_context *, Synth *);

double sine_oscillator(double time, double frequency, double amplitude);

double square_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
);

double sawtooth_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
);


double triangle_oscillator(
    double time,
    double frequency,
    double amplitude,
    int num_harmonics
);

double white_noise(double amplitude);
