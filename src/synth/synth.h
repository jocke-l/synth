#pragma once

typedef struct {
    _Atomic double square_frequency;
    _Atomic double square_amplitude;
    _Atomic int square_harmonics;

    _Atomic double inner_time_warp_frequency;
    _Atomic double inner_time_warp_amplitude;

    _Atomic double outer_time_warp_frequency;
    _Atomic double outer_time_warp_amplitude;
} Synth;

double synth_callback(double time, const void* synth_context);

struct nk_context;
void synth_update_ui(struct nk_context *, Synth *);
