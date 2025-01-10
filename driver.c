#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "Components/CPU/cpu.h"
#include "Components/Parser/load_rom.h"
#include "Components/Display/display.h"

#define S_TO_NS 1000000000L

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

    pthread_mutex_init(&lock, NULL);
    pthread_t thread_60hz;
    pthread_create(&thread_60hz, NULL, timers_60Hz, c);

    const double cycle_time = 1428571; // roughly 700 ins/s
    struct timespec start, end;  
    bool quit = false;
    while (!quit)
    {
        clock_gettime(CLOCK_MONOTONIC, &start);

        SDL_Event event;
        if (SDL_PollEvent(&event) && 
            event.type == SDL_QUIT) quit = true;
        WORD ins = cpu_fetch(c);
        cpu_exec(c, ins, quit, &window, &surface);
        if (cpu_has_drawn(c)) SDL_UpdateWindowSurface(window);

        clock_gettime(CLOCK_MONOTONIC, &end);

        unsigned long elapsed_time = (end.tv_sec - start.tv_sec) * S_TO_NS + (end.tv_nsec - start.tv_nsec);
        if (elapsed_time < cycle_time) {
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            while (elapsed_time >= S_TO_NS)
            {
                sleep_time.tv_sec += 1;
                elapsed_time -= S_TO_NS;
            }
            sleep_time.tv_nsec = elapsed_time;
            nanosleep(&sleep_time, NULL);
        }
    }

    quit_60Hz_thread = true;
    pthread_join(thread_60hz, NULL);

    pthread_mutex_destroy(&lock);
    destroy_SDL(&window);
    cpu_destroy(c);
    return 0;
}
