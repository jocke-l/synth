#include <stdio.h>
#include <stdlib.h>

#include <jack/jack.h>
#include <jack/types.h>

#include "common.h"

typedef struct {
    jack_port_t **output;
    jack_client_t *client;
    SynthCallback synth_callback;
} ProcessCallbackArgs;

int jack_process_callback(jack_nframes_t nframes, void *args) {
    ProcessCallbackArgs *process_args = args;

    jack_nframes_t current_frames;
    jack_time_t current_usecs, next_usecs;
    float period_usecs;
    jack_get_cycle_times(
        process_args->client,
        &current_frames,
        &current_usecs,
        &next_usecs,
        &period_usecs
    );

    jack_default_audio_sample_t *output_buffer[2];
    output_buffer[0] = jack_port_get_buffer(process_args->output[0], nframes);
    output_buffer[1] = jack_port_get_buffer(process_args->output[1], nframes);

    double frame_usecs = period_usecs / nframes;
    for (int i = 0; i < nframes; i++) {
        output_buffer[0][i] = output_buffer[1][i] =
            (float) process_args->synth_callback(
                ((current_usecs + frame_usecs * i) / 1000000.0)
            );
    }

    return 0;
}

void jack_shutdown_callback(void *args) {
}

int jack_connect_to_physical_output(jack_client_t *client, jack_port_t **output) {
    const char **physical_output = jack_get_ports(
        client,
        NULL,
        NULL,
        JackPortIsPhysical|JackPortIsInput
    );

    if (physical_output == NULL) {
        fprintf(stderr, "No physical playback ports\n");
        return 1;
    }

    for (int i = 0; i < 2; i++) {
        if (jack_connect(client, jack_port_name(output[i]), physical_output[i])) {
            fprintf(stderr, "Cannot connect output ports\n");
            free(physical_output);
            return 1;
        }
    }

    free(physical_output);

    return 0;
}

jack_port_t **jack_create_output(jack_client_t *client) {
    jack_port_t **output;

    if ((output = malloc(sizeof(jack_port_t *) * 2)) == NULL) {
        fprintf(stderr, "Out of memory");
        return NULL;
    }
    
    output[0] = jack_port_register(
        client,
        "output_left",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput, 
        0
    );
    output[1] = jack_port_register(
        client,
        "output_right",
        JACK_DEFAULT_AUDIO_TYPE,
        JackPortIsOutput, 
        0
    );

    if (output[0] == NULL || output[1] == NULL) {
        fprintf(stderr, "No more JACK ports available\n");
        return NULL;
    }

    return output;
}

int jack_init_client(char *client_name, SynthCallback synth_callback) {
    jack_status_t status;
    jack_client_t *client = jack_client_open(
        client_name,
        JackNullOption,
        &status, 
        NULL
    );
    if (client == NULL) {
        fprintf(
            stderr,
            "jack_client_open() failed, status = 0x%2.0x\n",
            status
        );
        if (status & JackServerFailed) {
            fprintf(stderr, "Unable to connect to JACK server\n");
        }
        return 1;
    }

    ProcessCallbackArgs *process_args;
    if ((process_args = malloc(sizeof(ProcessCallbackArgs))) == NULL) {
        fprintf(stderr, "Out of memory\n");
        return 1;
    }
    process_args->synth_callback = synth_callback;
    process_args->client = client;

    jack_set_process_callback(client, jack_process_callback, process_args);
    jack_on_shutdown(client, jack_shutdown_callback, 0);

    jack_port_t **output;
    if ((output = jack_create_output(client)) == NULL) {
        return 1;
    }
    process_args->output = output;

    if (jack_activate(client)) {
        fprintf(stderr, "Cannot activate client\n");
        return 1;
    }

    if (jack_connect_to_physical_output(client, output)) {
        return 1;
    }

    return 0;
}

