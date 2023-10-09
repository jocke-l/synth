#include "unreachable.h"

#include <assert.h>
#include <audio/audio.h>
#include <nuklear/nuklear.h>
#include <synth/synth.h>
#include <ui/ui.h>
#include <ui/uikey.h>

static void on_quit_event(void*);
static void on_key_up_event(void*, UIKey);
static void on_key_down_event(void*, UIKey);

int main(void) {
    Synth synth = (Synth) {
        .square_amplitude = 0.3,
        .square_k_value = 2.0,
        .square_harmonics = 20,

        .inner_time_warp_frequency = 0.05,
        .inner_time_warp_amplitude = 1.0,

        .outer_time_warp_frequency = 55.0,
        .outer_time_warp_amplitude = 0.015,
    };

    AudioClient* audio_client = audio_client_create("synth", synth_callback, &synth);
    if (!audio_client) {
        return -1;
    }

    if (ui_init() < 0) {
        return -1;
    }

    const unsigned window_width = 280;
    const unsigned window_height = 300;
    UI* ui = ui_create("synth", window_width, window_height);
    if (!ui) {
        ui_deinit();
        audio_client_destroy(audio_client);
        return -1;
    }

    int should_quit = 0;
    ui_set_on_quit_event(ui, on_quit_event, &should_quit);
    ui_set_on_key_up_event(ui, on_key_up_event, &synth);
    ui_set_on_key_down_event(ui, on_key_down_event, &synth);

    while(!should_quit) {
        ui_handle_events(ui);

        struct nk_context *nuklear_context = ui_get_nuklear_context(ui);
        if (nk_begin(nuklear_context, "synth", nk_rect(0, 0, window_width, window_height), 0)) {
            synth_update_ui(nuklear_context, &synth);
        }
        nk_end(nuklear_context);

        ui_render_frame(ui);
    }
    ui_destroy(ui);
    ui_deinit();
    audio_client_destroy(audio_client);
    return 0;
}

static void on_quit_event(void *context)
{
    *(int*)context = 1;
}

static bool should_handle_key_press(UIKey key);
static SynthKey ui_key_to_synth_key(UIKey key);

void on_key_down_event(void* context, UIKey key)
{
    Synth* synth = (Synth*)context;
    if (!should_handle_key_press(key))
        return;
    synth_set_key(synth, ui_key_to_synth_key(key));
}

void on_key_up_event(void* context, UIKey key)
{
    Synth* synth = (Synth*)context;
    if (!should_handle_key_press(key))
        return;
    synth_unset_key(synth, ui_key_to_synth_key(key));
}

static bool should_handle_key_press(UIKey key)
{
    if (key >= UIKey_0 && key <= UIKey_9)
        return true;
    if (key >= UIKey_a && key <= UIKey_z)
        return true;

    switch (key) {
    case UIKey_SEMICOLON: 
    case UIKey_COMMA:
    case UIKey_PERIOD:
    case UIKey_MINUS:
        return true;
    default: return false;
    }
}

static SynthKey ui_key_to_synth_key(UIKey key)
{
    assert(should_handle_key_press(key));
    switch(key) {
    case UIKey_1: return SynthKey_C4;
    case UIKey_2: return SynthKey_Db4;
    case UIKey_3: return SynthKey_D4;
    case UIKey_4: return SynthKey_Eb4;
    case UIKey_5: return SynthKey_E4;
    case UIKey_6: return SynthKey_F4;
    case UIKey_7: return SynthKey_Gb4;
    case UIKey_8: return SynthKey_G4;
    case UIKey_9: return SynthKey_Ab4;
    case UIKey_0: return SynthKey_A4;
    // SynthKey_Bb2;
    // SynthKey_B2,

    case UIKey_q: return SynthKey_C4;
    case UIKey_w: return SynthKey_Db4;
    case UIKey_e: return SynthKey_D4;
    case UIKey_r: return SynthKey_Eb4;
    case UIKey_t: return SynthKey_E4;
    case UIKey_y: return SynthKey_F4;
    case UIKey_u: return SynthKey_Gb4;
    case UIKey_i: return SynthKey_G4;
    case UIKey_o: return SynthKey_Ab5;
    case UIKey_p: return SynthKey_A4;
    // SynthKey_Bb3;
    // SynthKey_B3,

    case UIKey_a: return SynthKey_C3;
    case UIKey_s: return SynthKey_Db3;
    case UIKey_d: return SynthKey_D3;
    case UIKey_f: return SynthKey_Eb3;
    case UIKey_g: return SynthKey_E3;
    case UIKey_h: return SynthKey_F3;
    case UIKey_j: return SynthKey_Gb3;
    case UIKey_k: return SynthKey_G3;
    case UIKey_l: return SynthKey_Ab3;
    // SynthKey_A3;
    // SynthKey_Bb4;
    // SynthKey_B4,

    case UIKey_z: return SynthKey_C2;
    case UIKey_x: return SynthKey_Db2;
    case UIKey_c: return SynthKey_D2;
    case UIKey_v: return SynthKey_Eb2;
    case UIKey_b: return SynthKey_E2;
    case UIKey_n: return SynthKey_F2;
    case UIKey_m: return SynthKey_Gb2;
    case UIKey_COMMA:  return SynthKey_G2;
    case UIKey_PERIOD: return SynthKey_Ab2;
    case UIKey_MINUS:  return SynthKey_A2;
    // SynthKey_Bb5;
    // SynthKey_B5,

    default: unreachable();
    }
}
