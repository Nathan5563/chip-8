CC=gcc
CFLAGS=-g3 -Wall -pedantic
LDFLAGS=$(shell pkg-config --libs sdl2)

all: chip8

chip8: OBJs/cpu.o OBJs/display.o OBJs/stack.o OBJs/load_rom.o OBJs/map.o OBJs/mem.o OBJs/driver.o
	${CC} ${CFLAGS} -o $@ $^ ${LDFLAGS}

OBJs/cpu.o: Components/CPU/cpu.c Components/CPU/cpu.h
	${CC} ${CFLAGS} -c Components/CPU/cpu.c -o OBJs/cpu.o

OBJs/display.o: Components/Display/display.c Components/Display/display.h
	${CC} ${CFLAGS} -c Components/Display/display.c -o OBJs/display.o

OBJs/stack.o: Components/Stack/stack.c Components/Stack/stack.h
	${CC} ${CFLAGS} -c Components/Stack/stack.c -o OBJs/stack.o

OBJs/load_rom.o: Components/Parser/load_rom.c Components/Parser/load_rom.h
	${CC} ${CFLAGS} -c Components/Parser/load_rom.c -o OBJs/load_rom.o

OBJs/map.o: Components/Map/map.c Components/Map/map.h
	${CC} ${CFLAGS} -c Components/Map/map.c -o OBJs/map.o

OBJs/mem.o: Components/Memory/mem.c Components/Memory/mem.h
	${CC} ${CFLAGS} -c Components/Memory/mem.c -o OBJs/mem.o

OBJs/driver.o: driver.c Components/CPU/cpu.h Components/Parser/load_rom.h
	${CC} ${CFLAGS} -c driver.c -o driver.o
	mv driver.o OBJs

clean:
	rm -f chip8 OBJs/*.o driver.o