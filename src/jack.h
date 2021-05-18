#ifndef __JACK_H__
#define __JACK_H__

#include "common.h"

int jack_init_client(char *client_name, SynthCallback synth_callback);

#endif
