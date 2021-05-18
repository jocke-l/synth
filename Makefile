synth: $(wildcard src/*.c) $(wildcard src/*.h)
	gcc src/*.c -ljack -lm -o synth
