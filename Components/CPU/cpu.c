#include <stdlib.h>
#include <stdbool.h>

#include "cpu.h"
#include "../Display/display.h"
#include "../Stack/stack.h"
#include "../Map/map.h"
#include "../Memory/mem.h"
#include "../font.h"

#define FONT_SIZE 80

struct _cpu
{
    WORD PC;
    WORD I;
    stack* Stack;
    BYTE Delay, Sound;
    BYTE V[16];
};

void *copy(const void *src) {
    if (!src) return NULL;
    WORD *copy = malloc(sizeof(WORD));
    if (!copy) return NULL;
    *copy = *(const WORD *)src;
    return copy;
}

cpu* cpu_init()
{
    cpu* c = malloc(sizeof(cpu));
    c->Stack = stack_create(copy);
    c->PC = 0x0200;
    c->I = 0x0000;
    c->Delay = 0x00;
    c->Sound = 0x00;
    clear_screen();
    clear_mem();
    for (size_t i = 0; i < FONT_SIZE; ++i)
    {
        mem[i + 0x50] = font[i];
    }
    for (size_t i = 0; i < 16; ++i)
    {
        c->V[i] = 0x00;
    }
    return c;
}

WORD cpu_fetch(cpu* c)
{
    BYTE b1 = mem[c->PC++]; // eg. 00
    BYTE b2 = mem[c->PC++]; // eg. E0
    WORD ins = (b1 << 8) | b2; // eg. 00E0
    return ins;
}

void cpu_exec(cpu* c, WORD ins, bool quit, SDL_Window** window, SDL_Surface** surface)
{
    int reg;
    switch(ins & 0xF000)
    {
        case 0x0000:
            clear_screen();
            break;
        case 0x1000:
            c->PC = (ins & 0x0FFF);
            break;
        case 0x6000:
            reg = ((ins & 0x0F00) >> 8);
            c->V[reg] = (ins & 0x00FF);
            break;
        case 0x7000:
            reg = ((ins & 0x0F00) >> 8);
            c->V[reg] += (ins & 0x00FF);
            break;
        case 0xA000:
            c->I = (ins * 0x0FFF);
            break;
        case 0xD000:
            draw(window, surface); // and other params
            break;
        default:
            quit = true;
            break;
    }
}

void cpu_destroy(cpu* c)
{
    stack_destroy(c->Stack);
    free(c);
}