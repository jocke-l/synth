#include "synth.h"

#include <nuklear/nuklear.h>

void synth_update_ui(struct nk_context *ctx, Synth *synth) {

    nk_layout_row_dynamic(ctx, 25, 1);
    synth->square_amplitude = nk_propertyf(ctx, "square amplitude", 0, synth->square_amplitude, 1.0f, 0.01f, 0.01f);
    synth->square_k_value = nk_propertyf(ctx, "square k-value", 0, synth->square_k_value, 100.0f, 0.01f, 0.01f);
    int square_harmonics = synth->square_harmonics;
    nk_property_int(ctx, "square harmonics", 0, &square_harmonics, 100, 1, 1.0f);
    synth->square_harmonics = square_harmonics;

    synth->inner_time_warp_frequency = nk_propertyf(ctx, "inner time warp frequency", 0.0f, synth->inner_time_warp_frequency, 1.0f, 0.01f, 0.01f);
    synth->inner_time_warp_amplitude = nk_propertyf(ctx, "inner time warp amplitude", 0.0f, synth->inner_time_warp_amplitude, 1.0f, 0.01f, 0.01f);

    synth->outer_time_warp_frequency = nk_propertyf(ctx, "outer time warp frequency", 0.0f, synth->outer_time_warp_frequency, 20000.0f, 1.0f, 1.0f);
    synth->outer_time_warp_amplitude = nk_propertyf(ctx, "outer time warp amplitude", 0.0f, synth->outer_time_warp_amplitude, 1.0f, 0.001f, 0.001f);
}
