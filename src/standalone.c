#include <stdio.h>

#include <unistd.h>

#include "audio.h"
#include "synth.h"

int main(int argc, char **argv) {
    if (audio_init_client("synth", synth_callback)) {
        // Some logging
    }

    sleep(-1);

    return 0;
}
