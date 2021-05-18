#ifndef __SYNTH_H__
#define __SYNTH_H__

float synth_callback(double time);

float sine_oscillator(double time, float frequency, float amplitude);

float square_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
);

float sawtooth_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
);


float triangle_oscillator(
    double time,
    float frequency,
    float amplitude,
    int num_harmonics
);

float white_noise(float amplitude);

#endif
