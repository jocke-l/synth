#include "audio.h"
#include "nuklear.h"
#include "synth.h"
#include "ui.h"

static void on_quit_event(void*);

int main(void) {
    Synth synth = (Synth) {
        .volume = 0.3,
    };

    AudioClient* audio_client = audio_client_create("synth", synth_callback, &synth);
    if (!audio_client)
        goto fin_0;

    if (ui_init() < 0)
        goto fin_1;
    UI* ui = ui_create("standalone", 1200, 800);
    if (!ui)
        goto fin_2;

    int should_quit = 0;
    ui_set_on_quit_event(ui, on_quit_event, &should_quit);
    while(!should_quit) {
        ui_handle_events(ui);

        struct nk_context *nuklear_context = ui_get_nuklear_context(ui);
        if (nk_begin(nuklear_context, "synth", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
            NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE
        )) {
            synth_update_ui(nuklear_context, &synth);
        }
        nk_end(nuklear_context);

        ui_render_frame(ui);
    }
    ui_destroy(ui);
    ui_deinit();
    audio_client_destroy(audio_client);
    return 0;

fin_2: ui_deinit();
fin_1: audio_client_destroy(audio_client);
fin_0: return -1;
}

static void on_quit_event(void *context)
{
    *(int*)context = 1;
}
