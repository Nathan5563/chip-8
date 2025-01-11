#ifndef __CPU_H__
#define __CPU_H__

#include <stdbool.h>
#include <SDL2/SDL.h>

#include "../types.h"

struct _cpu;
typedef struct _cpu cpu;

extern bool quit_60Hz_thread;

cpu* cpu_init();

WORD cpu_get_index_reg(cpu* c);

bool cpu_has_drawn(cpu* c);

WORD cpu_fetch(cpu* c);

void cpu_exec(cpu* c, WORD ins, bool quit, SDL_Window** window, SDL_Surface** surface);

void cpu_destroy(cpu* c);

void cpu_decrement_timers(cpu* c);

void cpu_toggle_flag(cpu* c, bool enable);

#endif