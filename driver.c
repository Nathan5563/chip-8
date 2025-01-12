#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <sys/stat.h>

#include <SDL2/SDL.h>

#include "Components/CPU/cpu.h"
#include "Components/Parser/load_rom.h"
#include "Components/Display/display.h"

#define S_TO_NS 1000000000

#define FREQUENCY 75000

/**
 * Thread globals
 */
pthread_mutex_t lock;
bool quit_60Hz_thread = false;

void* timers_60Hz(void* arg) {
    pthread_mutex_lock(&lock);
    while (!quit_60Hz_thread) {
        usleep(16666);  // Sleep for ~16.67ms (1/60th of a second)
        cpu_decrement_timers((cpu*)arg);
    }
    pthread_mutex_unlock(&lock);
    return NULL;
}

FILE* validate_cli(int argc, char* argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "chip8: Invalid input.\n");
        fprintf(stdout, "------------------------------------------------------------\n");
        fprintf(stdout, "Usage: ./chip8 path\n");
        fprintf(stdout, "where 'path' is the full path to the desired CHIP-8 program.\n");
        fprintf(stdout, "------------------------------------------------------------\n");
        return NULL;
    }
    struct stat statbuf;
    if ((stat(argv[1], &statbuf) != 0) || !S_ISREG(statbuf.st_mode)) 
    {
        fprintf(stderr, "chip8: The provided file '%s' could not be found.\n", argv[1]);
        return NULL;
    }
    size_t arg_len = strlen(argv[1]);
    if ((argv[1][arg_len - 4] != '.') ||
        (argv[1][arg_len - 3] != 'c') ||
        (argv[1][arg_len - 2] != 'h') ||
        (argv[1][arg_len - 1] != '8'))
    {
        fprintf(stderr, "chip8: The provided file '%s' is not a valid CHIP-8 ROM.\n", argv[1]);
        return NULL;
    }
    FILE* rom = fopen(argv[1], "rb");
    if (!rom)
    {
        fprintf(stderr, "chip8: The provided file '%s' could not be opened.\n", argv[1]);
        return NULL;
    }
    return rom;
}

struct timespec get_time_diff(unsigned long time, unsigned long cycle)
{
    unsigned long diff = cycle - time;
    struct timespec time_diff;
    time_diff.tv_sec = floor(diff/S_TO_NS);
    diff -= (time_diff.tv_sec * S_TO_NS);
    time_diff.tv_nsec = diff;
    return time_diff;
}

void main_loop(cpu* c, SDL_Window** window, SDL_Surface** surface)
{
    unsigned long cycle_time = S_TO_NS/FREQUENCY;
    struct timespec start, end;  
    bool quit = false;
    while (!quit)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);

        SDL_Event event;
        if (SDL_PollEvent(&event) && 
            event.type == SDL_QUIT) quit = true;
        WORD ins = cpu_fetch(c);
        cpu_exec(c, ins, quit, window, surface);
        if (cpu_has_drawn(c)) SDL_UpdateWindowSurface(*window);

        clock_gettime(CLOCK_MONOTONIC, &end);

        // sleep the time difference if loop is executed too quickly
        // to match the required cycle time
        unsigned long elapsed_time = (end.tv_sec - start.tv_sec) * S_TO_NS + (end.tv_nsec - start.tv_nsec);
        if (elapsed_time < cycle_time) {
            struct timespec sleep_time = get_time_diff(elapsed_time, cycle_time);
            nanosleep(&sleep_time, NULL);
        }
    }
}

void cleanup(cpu* c, SDL_Window** window, pthread_t* thread)
{
    quit_60Hz_thread = true;
    pthread_join(*thread, NULL);
    pthread_mutex_destroy(&lock);
    destroy_SDL(window);
    cpu_destroy(c);
}

int main(int argc, char* argv[]) {
    FILE* rom = validate_cli(argc, argv);
    if (!rom) return 1;

    cpu* c = cpu_init();
    load_rom(rom);
    fclose(rom);

    SDL_Window* window = NULL;
    SDL_Surface* surface = NULL;
    initialize_SDL(&window, &surface);

    pthread_mutex_init(&lock, NULL);
    pthread_t thread_60hz;
    pthread_create(&thread_60hz, NULL, timers_60Hz, c);

    main_loop(c, &window, &surface);
    cleanup(c, &window, &thread_60hz);
    return 0;
}
