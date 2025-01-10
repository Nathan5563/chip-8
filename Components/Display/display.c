#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "../CPU/cpu.h"
#include "../Map/map.h"

void print_error(const char* err)
{
    fprintf(stderr, "SDL Error: %s\n", err);
}

// else
// {
//     bool quit = false;  
//     while (!quit)
//     {
//         if (!draw(&window, &surface)) quit = true;
//         SDL_Event event;
//         if (SDL_PollEvent(&event) && 
//             event.type == SDL_QUIT) quit = true;
//     }
// }

void initialize_SDL(SDL_Window** window, SDL_Surface** surface)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        print_error(SDL_GetError());
    else
    {
        *window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_RESIZABLE);
        if(!*window) print_error(SDL_GetError());
        else
        {
            *surface = SDL_GetWindowSurface(*window);
            if (!*surface) print_error(SDL_GetError());
        }
    }
}

// starting x position of draw is modulo 64
// starting y position of draw is modulo 32
// find a way to convert the bytes x and y to ints
// for each n in height, lookup the nth byte including and after the one pointed to by I
// figure out what to do if I is pointing to a WORD
// XOR each bit in each byte with bits starting from (x, y) and ending on (64, y) on the screen, 
//  or until sprite data runs out
// stop drawing when position (64, 32) is reached
void draw(cpu* c, SDL_Window** window, SDL_Surface** surface, BYTE x, BYTE y, int height)
{
    SDL_Rect rect;
    rect.w = GRID_SIZE;
    rect.h = GRID_SIZE;
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        rect.y = 0 + (y * GRID_SIZE);
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            rect.x = 0 + (x * GRID_SIZE);
            if (map[y * MAP_WIDTH + x] == 1)
            {
                if (SDL_FillRect(*surface, &rect, SDL_MapRGB(
                    (*surface)->format, 255, 255, 255)))
                {
                    print_error(SDL_GetError());
                }
            }
        }
    }
}

void destroy_SDL(SDL_Window** window)
{
    SDL_DestroyWindow(*window);
    SDL_Quit();
}