CC=gcc
CFLAGS=-g3 -Wall -pedantic -lm -I.
LDFLAGS=$(shell pkg-config --libs sdl2)

# Specify directories where source files reside
VPATH=.:Components/CPU:Components/Display:Components/Parser:Components/Stack:Components/Map:Components/Memory:Components/Quirks:Components/Font:Components/Timer

# List of object files
OBJS=OBJs/driver.o OBJs/cpu.o OBJs/display.o OBJs/load_rom.o OBJs/stack.o OBJs/map.o OBJs/mem.o OBJs/quirks.o OBJs/font.o OBJs/timer.o

chip8: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# Generic rule: compile any source file from VPATH into OBJs
OBJs/%.o: %.c
	@mkdir -p OBJs
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f chip8 OBJs/*.o