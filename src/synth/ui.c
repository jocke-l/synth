#include "synth.h"

#include <nuklear/nuklear.h>

void synth_update_ui(struct nk_context *ctx, Synth *synth) {
    nk_layout_row_dynamic(ctx, 25, 1);
    synth->volume = nk_propertyf(ctx, "Volume", 0, synth->volume, 1.0f, 0.01f, 0.005f);
}
