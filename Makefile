synth: $(wildcard src/*.c) $(wildcard src/*.h) src/Nuklear/nuklear.h
	gcc src/*.c -ljack -lm -lSDL2 -o synth

src/Nuklear/nuklear.h:
	git submodule update --recursive --init
