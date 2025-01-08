#include <stdio.h>
#include <stdlib.h>

#include "Components/CPU/cpu.h"
#include "Components/Parser/load_rom.h"

// call cpu_init
// read which rom to load from cli
// call read_rom on opened file
// close file
// call cpu execute on timed while loop
// figure out how to link display
//   start display on init, if DXYN is used find a way to XOR world_map
//   and update in real time
// figure out timers
//   have a larger loop at 700 ins cycles per sec, do some math with counters
//   to have an if statement inside that runs at 60hz

int main(int argc, char* argv[]) {
    // TODO: validate cli input (with error handling)
    if (!argv[1])
    {
        fprintf(stderr, "Please provide a CHIP-8 ROM.\n");
        return 1;
    }
    cpu* c = cpu_init();
    FILE* rom = fopen(argv[1], "rb");
    load_rom(rom);
    fclose(rom);
    cpu_destroy(c);
    return 0;
}
