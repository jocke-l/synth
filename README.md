synth
=====

This is a simple synthesizer that aims to reproduce the sounds of analog synthesizers.

## How to compile

Currently only JACK is supported. Both the original JACK server and PipeWire's
JACK server will work.

```shell
$ make
```
*Note: You might want to lower the sound volume as it will start playing immediately.*

You can then run the produced binary with `./synth`.
