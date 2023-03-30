#include <stdio.h>

#include <unistd.h>

#include "audio.h"
#include "synth.h"

int main(int argc, char **argv) {
    Synth synth = (Synth) {
        .volume = 0.3,
    };

    if (audio_init_client("synth", synth_callback, &synth)) {
        // Some logging
    }

    sleep(-1);

    return 0;
}
