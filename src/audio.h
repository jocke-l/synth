#pragma once

#include "common.h"

int audio_init_client(char *client_name, SynthCallback synth_callback, const void *synth_context);
