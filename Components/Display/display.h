#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#include "../CPU/cpu.h"
#include "../types.h"

#define MAP_WIDTH 64
#define MAP_HEIGHT 32
#define GRID_SIZE 20
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640

void print_error(const char *err);

void initialize_SDL(SDL_Window** window, SDL_Surface** surface);

void draw(cpu* c, SDL_Window** window, SDL_Surface** surface, BYTE x, BYTE y, int height);

void destroy_SDL(SDL_Window** window);

#endif