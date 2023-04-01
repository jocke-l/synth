#include <audio/audio.h>
#include <nuklear/nuklear.h>
#include <synth/synth.h>
#include <ui/ui.h>

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

    const unsigned window_width = 280;
    const unsigned window_height = 300;
    UI* ui = ui_create("synth", window_width, window_height);
    if (!ui)
        goto fin_2;

    int should_quit = 0;
    ui_set_on_quit_event(ui, on_quit_event, &should_quit);
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

fin_2: ui_deinit();
fin_1: audio_client_destroy(audio_client);
fin_0: return -1;
}

static void on_quit_event(void *context)
{
    *(int*)context = 1;
}
