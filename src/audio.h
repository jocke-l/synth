#pragma once

#include "common.h"

typedef struct AudioClient AudioClient;
AudioClient *audio_client_create(const char *client_name, SynthCallback synth_callback, const void *synth_context);
void audio_client_destroy(AudioClient *);
