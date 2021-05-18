#include <stdio.h>

#include <unistd.h>

#include "jack.h"
#include "synth.h"

int main(int argc, char **argv) {
    if (jack_init_client("synth", synth_callback)) {
        // Some logging
    }

    sleep(-1);

    return 0;
}
