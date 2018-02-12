BIN := bin/chip8emu

CC := gcc
STD := -std=c89
EXT := .c

SRCS := $(wildcard src/*$(EXT)) $(wildcard src/*/*$(EXT))
OBJS := $(addprefix obj/, $(notdir $(SRCS:$(EXT)=.o)))

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS  := $(STD) -pedantic -Wall -F/Library/Frameworks
	LDFLAGS := -framework SDL2 -lm -F/Library/Frameworks
endif

ifeq ($(UNAME),Linux)
	CFLAGS  := $(STD) -pedantic -Wall `pkg-config --cflags sdl2` -lm
	LDFLAGS := `pkg-config --libs sdl2` -lm
endif

all: $(BIN)
clean: $(shell rm -rf $(BIN) $(OBJS))

obj/%.o: src/%$(EXT)
	$(CC) -c -o $@ $< $(CFLAGS)

obj/%.o: src/*/%$(EXT)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)
