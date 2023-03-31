#pragma once

typedef struct {
    _Atomic double volume;
} Synth;

double synth_callback(double time, const void* synth_context);

struct nk_context;
void synth_update_ui(struct nk_context *, Synth *);
