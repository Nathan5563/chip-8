#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "Components/CPU/cpu.h"
#include "Components/Parser/load_rom.h"
#include "Components/Display/display.h"

// figure out how to link display
//   start display on init, if DXYN is used find a way to XOR world_map
//   and update in real time
// figure out timers
//   have a larger loop at 700 ins cycles per sec, do some math with counters
//   to have an if statement inside that runs at 60hz
//   or, have while(true) and figure out how to run emulate_cycle at 700hz and
//   decrement timers at 60hz

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

    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;
    initialize_SDL(&window, &surface);

    bool quit = false;  
    while (!quit)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event) && 
            event.type == SDL_QUIT) quit = true;
        WORD ins = cpu_fetch(c);
        cpu_exec(c, ins, quit, &window, &surface);
    }

    destroy_SDL(&window);
    cpu_destroy(c);
    return 0;
}
