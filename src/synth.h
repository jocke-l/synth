#ifndef __SYNTH_H__
#define __SYNTH_H__

double synth_callback(double time);

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

#endif
