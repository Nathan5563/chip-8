#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "display.h"

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
        print_error(SDL_GetError());
    else
    {
        SDL_Window* window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT,
                SDL_WINDOW_RESIZABLE);
        if(!window) print_error(SDL_GetError());
        else
        {
            SDL_Surface* surface = SDL_GetWindowSurface(window);
            if (!surface) print_error(SDL_GetError());
            else
            {
                bool quit = false;  
                while (!quit)
                {
                    if (!draw(&window, &surface)) quit = true;
                    SDL_Event event;
                    if (SDL_PollEvent(&event) && 
                        event.type == SDL_QUIT) quit = true;
                }
            }
            SDL_DestroyWindow(window);
        }
        SDL_Quit();
    }
    return 0;
}

void print_error(const char* err)
{
    fprintf(stderr, "SDL Error: %s\n", err);
}

bool draw(SDL_Window** window, SDL_Surface** surface)
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
            if (world_map[y * MAP_WIDTH + x] == 1)
            {
                if (SDL_FillRect(*surface, &rect, SDL_MapRGB(
                    (*surface)->format, 255, 255, 255)))
                {
                    print_error(SDL_GetError());
                    return false;
                }
            }
        }
    }
    if (SDL_UpdateWindowSurface(*window)) 
    {
        print_error(SDL_GetError());
        return false;
    }
    return true;
}
