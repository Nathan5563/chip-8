#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

#include "cpu.h"
#include "../Display/display.h"
#include "../Stack/stack.h"
#include "../Map/map.h"
#include "../Memory/mem.h"
#include "../font.h"

#define FONT_SIZE 80

// TODO: Use lots of macros to prevent hex confusion

struct _cpu
{
    WORD PC;
    WORD I;
    stack* Stack;
    BYTE Delay, Sound;
    BYTE V[16];
    bool drawn;
};

cpu* cpu_init()
{
    cpu* c = malloc(sizeof(cpu));
    c->Stack = stack_create();
    c->PC = 0x0200;
    c->I = 0x0000;
    c->Delay = 0x00;
    c->Sound = 0x00;
    c->drawn = false;
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

WORD cpu_get_index_reg(cpu* c)
{
    return c->I;
}

bool cpu_has_drawn(cpu* c)
{
    return c->drawn;
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
    c->drawn = false;
    int X, Y, N;
    switch(ins & 0xF000)
    {
        case 0x0000:
            switch(ins & 0x00FF)
            {
                case 0x00E0:
                    clear_screen();
                    break;
                case 0x00EE:
                    c->PC = stack_pop(c->Stack);
                    break;
            }
            break;
        case 0x1000:
            c->PC = (ins & 0x0FFF);
            break;
        case 0x2000:
            stack_push(c->Stack, c->PC);
            c->PC = (ins & 0x0FFF);
            break;
        case 0x3000:
            X = ((ins & 0x0F00) >> 8);
            if (c->V[X] == (ins & 0x00FF)) c->PC += 2;
            break;
        case 0x4000:
            X = ((ins & 0x0F00) >> 8);
            if (c->V[X] != (ins & 0x00FF)) c->PC += 2;
            break;
        case 0x5000:
            X = ((ins & 0x0F00) >> 8);
            Y = ((ins & 0x00F0) >> 4);
            if (c->V[X] == c->V[Y]) c->PC += 2;
            break;
        case 0x6000:
            X = ((ins & 0x0F00) >> 8);
            c->V[X] = (ins & 0x00FF);
            break;
        case 0x7000:
            X = ((ins & 0x0F00) >> 8);
            c->V[X] += (ins & 0x00FF);
            break;
        case 0x8000:
            X = ((ins & 0x0F00) >> 8);
            Y = ((ins & 0x00F0) >> 4);
            switch(ins & 0x000F)
            {
                case 0x0000:
                    c->V[X] = c->V[Y];
                    break;
                case 0x0001:
                    c->V[X] |= c->V[Y];
                    break;
                case 0x0002:
                    c->V[X] &= c->V[Y];
                    break;
                case 0x0003:
                    c->V[X] ^= c->V[Y];
                    break;
                case 0x0004:
                    c->V[X] += c->V[Y];
                    if (c->V[X] + c->V[Y] >= 0x100) c->V[0xF] = 1;
                    else c->V[0xF] = 0;
                    break;
                case 0x0005:
                    c->V[X] -= c->V[Y];
                    if (c->V[X] > c->V[Y]) c->V[0xF] = 1;
                    else c->V[0xF] = 0;
                    break;
                case 0x0006:
                    // Ambiguous
                    break;
                case 0x0007:
                    c->V[X] = c->V[Y] - c->V[X];
                    if (c->V[Y] > c->V[X]) c->V[0xF] = 1;
                    else c->V[0xF] = 0;
                    break;
                case 0x000E:
                    // Ambiguous
                    break;
            }
        case 0x9000:
            X = ((ins & 0x0F00) >> 8);
            Y = ((ins & 0x00F0) >> 4);
            if (c->V[X] != c->V[Y]) c->PC += 2;
            break;
        case 0xA000:
            c->I = (ins & 0x0FFF);
            break;
        case 0xB000:
            // Ambiguous
            break;
        case 0xC000:
            X = ((ins & 0x0F00) >> 8);
            c->V[X] = (rand() % (0xFF - 0x00 + 1) + 0x00) & (ins & 0x00FF);
        case 0xD000:
            c->drawn = true;
            X = ((ins & 0x0F00) >> 8);
            Y = ((ins & 0x00F0) >> 4);
            N = (ins & 0x000F);
            draw(c, window, surface, c->V[X], c->V[Y], N);
            break;
        case 0xE000:
            X = ((ins & 0x0F00) >> 8);
            switch(ins & 0x00FF)
            {
                case 0x009E:
                    break;
                case 0x00A1:
                    break;
            }
        case 0xF000:
            X = ((ins & 0x0F00) >> 8);
            switch(ins & 0x00FF)
            {
                case 0x0007:
                    c->V[X] = c->Delay;
                    break;
                case 0x000A:
                    
                    break;
                case 0x0015:
                    c->Delay = c->V[X];
                    break;
                case 0x0018:
                    c->Sound = c->V[X];
                    break;
                case 0x001E:
                    c->I += c->V[X];
                    if (c->I + c->V[X] >= 0x1000) c->V[0xF] = 1;
                    // might need to set VF = 0 otherwise, but leave as is for now
                    break;
                case 0x0029:
                    // set I to the address where the character represented by the
                    // last nibble of VX is stored in the font (in memory)
                    break;
                case 0x0033:
                    break;
                case 0x0055:
                    // Ambiguous
                    break;
                case 0x0065:
                    // Ambiguous
                    break;
            }
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

void cpu_decrement_timers(cpu* c)
{
    if (c->Delay > 0) c->Delay--;
    if (c->Sound > 0) c->Sound--;
}

void cpu_toggle_flag(cpu* c, bool enable)
{
    if (enable)
    {
        c->V[0xF] = 1;
    }
    else
    {
        c->V[0xF] = 0;
    }
    return;
}
