#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "display.h"
#include "../CPU/cpu.h"
#include "../Map/map.h"
#include "../Memory/mem.h"

void print_error(const char* err)
{
    fprintf(stderr, "SDL Error: %s\n", err);
}

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

int get_index(int x, int y)
{
    return (y * 64) + x;
}

bool xor(int bit, cpu* c, BYTE* x, BYTE* y)
{
    int idx = get_index(*x, *y);
    if (idx % 64 == 0) return false;
    if (idx >= 2048) return false;
    if (bit && map[idx])
    {
        map[idx] = 0;
        cpu_toggle_flag(c, true);
        printf("Turned off!\n");
    }
    else if (bit)
    {
        map[idx] = 1;
        printf("Turned on!\n");
    }
    (*x)++;
    return true;
}

void foreach_bit(BYTE num, bool (*f)(int, cpu*, BYTE*, BYTE*), cpu* c, BYTE* x, BYTE* y)
{
    bool drawing;
    for (int i = 7; i >= 0; i--) 
    {
        int bit = (num & (1 << i)) ? 1 : 0;
        drawing = f(bit, c, x, y);
        if (!drawing) return;
    }
}

void draw(cpu* c, SDL_Window** window, SDL_Surface** surface, BYTE x, BYTE y, int height)
{
    BYTE x_pos = x & 63;
    BYTE y_pos = y & 31;
    WORD reg = cpu_get_index_reg(c);
    cpu_toggle_flag(c, false);
    for (size_t N = 0; N < height; ++N)
    {
        BYTE sprite_data = mem[reg + N];
        foreach_bit(sprite_data, xor, c, &x_pos, &y_pos);
        y_pos++;
        x_pos = x & 63;
    }
    
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