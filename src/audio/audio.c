#include "audio.h"

#include <stdio.h>
#include <stdlib.h>

#include <jack/jack.h>
#include <jack/types.h>

#define AUDIO_NUMBER_OF_OUTPUTS 2

struct AudioClient {
    jack_port_t **outputs;
    jack_client_t *jack_client;
    SynthCallback synth_callback;
    const void *synth_context;
} static audio_client_invalid = { 0 };

static int audio_process_callback(jack_nframes_t nframes, void *args) {
    AudioClient *audio_client = args;

    jack_nframes_t current_frames;
    jack_time_t current_usecs, next_usecs;
    float period_usecs;
    jack_get_cycle_times(
        audio_client->jack_client,
        &current_frames,
        &current_usecs,
        &next_usecs,
        &period_usecs
    );

    jack_default_audio_sample_t *output_buffer[2];
    output_buffer[0] = jack_port_get_buffer(audio_client->outputs[0], nframes);
    output_buffer[1] = jack_port_get_buffer(audio_client->outputs[1], nframes);

    double frame_usecs = period_usecs / nframes;
    for (unsigned int i = 0; i < nframes; i++) {
        output_buffer[0][i] = output_buffer[1][i] =
            (float) audio_client->synth_callback(
                ((current_usecs + frame_usecs * i) / 1000000.0), audio_client->synth_context
            );
    }

    return 0;
}

static void audio_shutdown_callback(void *args);

static int audio_connect_to_physical_output(jack_client_t *client, jack_port_t **outputs) {
    const char **physical_outputs = jack_get_ports(
        client,
        NULL,
        NULL,
        JackPortIsPhysical|JackPortIsInput
    );
    if (!physical_outputs) {
        fprintf(stderr, "No physical playback ports\n");
        return 1;
    }

    for (int i = 0; i < AUDIO_NUMBER_OF_OUTPUTS; i++) {
        if (jack_connect(client, jack_port_name(outputs[i]), physical_outputs[i])) {
            fprintf(stderr, "Cannot connect output ports\n");
            jack_free(physical_outputs);
            return 1;
        }
    }

    jack_free(physical_outputs);
    return 0;
}

static jack_port_t **audio_outputs_create(jack_client_t *client) {

    jack_port_t **outputs = malloc(sizeof(jack_port_t *) * AUDIO_NUMBER_OF_OUTPUTS);
    if (!outputs) {
        fprintf(stderr, "Out of memory");
        return NULL;
    }
    
    outputs[0] = jack_port_register(
        client,
        "output_left",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput, 
        0
    );
    if (!outputs[0]) {
        fprintf(stderr, "No more JACK ports available\n");
        free(outputs);
        return NULL;
    }

    outputs[1] = jack_port_register(
        client,
        "output_right",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput, 
        0
    );
    if (!outputs[1]) {
        fprintf(stderr, "No more JACK ports available\n");
        jack_port_unregister(client, outputs[0]);
        free(outputs);
        return NULL;
    }

    return outputs;
}

AudioClient *audio_client_create(const char *client_name, SynthCallback synth_callback, const void *synth_context) {
    AudioClient* audio_client = malloc(sizeof(AudioClient));
    if (!audio_client) {
        fprintf(stderr, "Out of memory\n");
        return NULL;
    }
    *audio_client = (AudioClient) {
        .synth_callback = synth_callback,
        .synth_context = synth_context,
    };

    jack_status_t status;
    audio_client->jack_client = jack_client_open(
        client_name,
        JackNullOption,
        &status, 
        NULL
    );
    if (!audio_client->jack_client) {
        fprintf(stderr, "jack_client_open() failed, status = 0x%2.0x\n", status);
        if (status & JackServerFailed) {
            fprintf(stderr, "Unable to connect to JACK server\n");
        }
        free(audio_client);
        return NULL;
    }

    jack_set_process_callback(audio_client->jack_client, audio_process_callback, audio_client);
    jack_on_shutdown(audio_client->jack_client, audio_shutdown_callback, audio_client);

    audio_client->outputs = audio_outputs_create(audio_client->jack_client);
    if (!audio_client->outputs) {
        jack_client_close(audio_client->jack_client);
        free(audio_client);
        return NULL;
    }

    if (jack_activate(audio_client->jack_client)) {
        fprintf(stderr, "Cannot activate client\n");
        jack_client_close(audio_client->jack_client);
        free(audio_client);
        return NULL;
    }

    if (audio_connect_to_physical_output(audio_client->jack_client, audio_client->outputs)) {
        jack_deactivate(audio_client->jack_client);
        jack_client_close(audio_client->jack_client);
        free(audio_client);
        return NULL;
    }

    return audio_client;
}

void audio_client_destroy(AudioClient *client) {
    jack_deactivate(client->jack_client);
    jack_client_close(client->jack_client);
    *client = audio_client_invalid;
    free(client);
}

static void audio_shutdown_callback(void *) {
}

static double g_sinewave_table[0x100000];
static const unsigned g_sinewave_table_size = sizeof(g_sinewave_table) / sizeof(g_sinewave_table[0]);
const double tau = 2 * 3.14159265359;

[[gnu::constructor]]
void initialize_sinewave_table(void)
{
    for (unsigned i = 0; i < g_sinewave_table_size; i++)
        g_sinewave_table[i] = __builtin_sin(tau * (i / (double)g_sinewave_table_size));
}

double audio_sin_turns(double value)
{
    const double fraction = value - (long long)value;
    return g_sinewave_table[(unsigned long long)(fraction * (double)g_sinewave_table_size) % g_sinewave_table_size];
}
