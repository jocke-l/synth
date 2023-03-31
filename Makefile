SOURCES = $(wildcard src/*.c)
OBJECTS = $(SOURCES:src/%.c=build/%.c.o)
DEPS = $(OBJECTS:%.o=%.d)

CCACHE = $(shell which ccache 2> /dev/null)
ifdef CCACHE
CC := $(CCACHE) $(CC)
else
$(warning "ccache not installed, consider installing it for faster rebuilds")
endif

ifndef ECHO
HIT_TOTAL != ${MAKE} ${MAKECMDGOALS} --dry-run ECHO="HIT_MARK" | grep -c "HIT_MARK"
HIT_COUNT = $(eval HIT_N != expr ${HIT_N} + 1)${HIT_N}
ECHO = echo "[${HIT_COUNT}/${HIT_TOTAL}]"
endif

all: synth

src/Nuklear/nuklear.h:
	@$(ECHO) "Fetch $@"
	@git submodule update --recursive --init > /dev/null

build:
	@mkdir -p build

build/%.c.o: src/%.c build src/Nuklear/nuklear.h
	@$(ECHO) "Compile $<"
	@$(CC) -MMD -c $< -o $@

synth: $(OBJECTS)
	@$(ECHO) "Link $@"
	@$(CC) $^ -ljack -lm -lSDL2 -o $@

clean:
	@$(RM) synth
	@$(RM) -r build

-include $(DEPS)
