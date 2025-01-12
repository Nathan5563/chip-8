#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#include "cpu.h"
#include "../Display/display.h"
#include "../Stack/stack.h"
#include "../Map/map.h"
#include "../Memory/mem.h"
#include "../Font/font.h"
#include "../types.h"
#include "../Quirks/quirks.h"

#define FONT_SIZE 80

// TODO: Use lots of macros to prevent hex confusion

#define FOURTH_BIT(word) ((word & 0xF000) >> 12)
#define THIRD_BIT(word) ((word & 0x0F00) >> 8)
#define SECOND_BIT(word) ((word & 0x00F0) >> 4)
#define FIRST_BIT(word) (word & 0x000F)

#define TRAILING_BITS(word) (word & 0x0FFF)

#define SECOND_BYTE(word) ((word & 0xFF00) >> 8)
#define FIRST_BYTE(word) (word & 0x00FF)

#define SECOND_NIBBLE(byte) ((byte & 0xF0) >> 4)
#define FIRST_NIBBLE(byte) (byte & 0x0F)

#define STARTING_LOCATION 0x200
#define FONT_LOCATION 0x50
#define BYTES_PER_CHAR 5

#define MEMORY_LIMIT 0x1000
#define BYTE_LIMIT 0x100

#define NUM_REGISTERS 16

#define HEX_TO_DEC(N) (FIRST_BIT(N) + SECOND_BIT(N) * 16 + THIRD_BIT(N) * 256 + FOURTH_BIT(N) * 4096)

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
    c->PC = STARTING_LOCATION;
    c->I = 0;
    c->Delay = 0;
    c->Sound = 0;
    c->drawn = false;
    clear_screen();
    clear_mem();
    for (size_t i = 0; i < FONT_SIZE; ++i)
    {
        mem[i + FONT_LOCATION] = font[i];
    }
    for (size_t i = 0; i < NUM_REGISTERS; ++i)
    {
        c->V[i] = 0;
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

static int get_scan_code(WORD k)
{
    int scan_code;
    switch(k)
    {
        case 0:
            scan_code = SDL_SCANCODE_0;
            break;
        case 1:
            scan_code = SDL_SCANCODE_1;
            break;
        case 2:
            scan_code = SDL_SCANCODE_2;
            break;
        case 3:
            scan_code = SDL_SCANCODE_3;
            break;
        case 4:
            scan_code = SDL_SCANCODE_4;
            break;
        case 5:
            scan_code = SDL_SCANCODE_5;
            break;
        case 6:
            scan_code = SDL_SCANCODE_6;
            break;
        case 7:
            scan_code = SDL_SCANCODE_7;
            break;
        case 8:
            scan_code = SDL_SCANCODE_8;
            break;
        case 9:
            scan_code = SDL_SCANCODE_9;
            break;
        case 0xA:
            scan_code = SDL_SCANCODE_A;
            break;
        case 0xB:
            scan_code = SDL_SCANCODE_B;
            break;
        case 0xC:
            scan_code = SDL_SCANCODE_C;
            break;
        case 0xD:
            scan_code = SDL_SCANCODE_D;
            break;
        case 0xE:
            scan_code = SDL_SCANCODE_E;
            break;
        case 0xF:
            scan_code = SDL_SCANCODE_F;
            break;
        default:
            scan_code = 0;
            break;
    }
    return scan_code;
}

WORD get_key_down()
{
    WORD key;
    SDL_Event event;
    bool key_pressed = false;   
    while (!key_pressed) 
    {
        if (!SDL_PollEvent(&event)) continue;
        if (event.type == SDL_KEYUP) 
        {
            key_pressed = true;
            switch (event.key.keysym.scancode) 
            {
                case SDL_SCANCODE_0:
                    key = 0;
                    break;
                case SDL_SCANCODE_1:
                    key = 1;
                    break;
                case SDL_SCANCODE_2:
                    key = 2;
                    break;
                case SDL_SCANCODE_3:
                    key = 3;
                    break;
                case SDL_SCANCODE_4:
                    key = 4;
                    break;
                case SDL_SCANCODE_5:
                    key = 5;
                    break;
                case SDL_SCANCODE_6:
                    key = 6;
                    break;
                case SDL_SCANCODE_7:
                    key = 7;
                    break;
                case SDL_SCANCODE_8:
                    key = 8;
                    break;
                case SDL_SCANCODE_9:
                    key = 9;
                    break;
                case SDL_SCANCODE_A:
                    key = 0xA;
                    break;
                case SDL_SCANCODE_B:
                    key = 0xB;
                    break;
                case SDL_SCANCODE_C:
                    key = 0xC;
                    break;
                case SDL_SCANCODE_D:
                    key = 0xD;
                    break;
                case SDL_SCANCODE_E:
                    key = 0xE;
                    break;
                case SDL_SCANCODE_F:
                    key = 0xF;
                    break;
                default:
                    key_pressed = false;
                    break;
            }
        }
        else if (event.type == SDL_QUIT)
        {
            key_pressed = true;
        }
    }
    return key;
}

// TODO: 0xE and 0xFX0A
void cpu_exec(cpu* c, WORD ins, bool quit, SDL_Window** window, SDL_Surface** surface)
{
    c->drawn = false;
    WORD X, Y, N;
    const uint8_t* keyboard_state;
    BYTE f;
    switch(FOURTH_BIT(ins))
    {
        case 0x0:
            switch(FIRST_BYTE(ins))
            {
                case 0xE0:
                    c->drawn = true;
                    clear_screen();
                    break;
                case 0xEE:
                    c->PC = stack_pop(c->Stack);
                    break;
            }
            break;
        case 0x1:
            c->PC = TRAILING_BITS(ins);
            break;
        case 0x2:
            stack_push(c->Stack, c->PC);
            c->PC = TRAILING_BITS(ins);
            break;
        case 0x3:
            X = THIRD_BIT(ins);
            if (c->V[X] == FIRST_BYTE(ins)) c->PC += 2;
            break;
        case 0x4:
            X = THIRD_BIT(ins);
            if (c->V[X] != FIRST_BYTE(ins)) c->PC += 2;
            break;
        case 0x5:
            X = THIRD_BIT(ins);
            Y = SECOND_BIT(ins);
            if (c->V[X] == c->V[Y]) c->PC += 2;
            break;
        case 0x6:
            X = THIRD_BIT(ins);
            c->V[X] = FIRST_BYTE(ins);
            break;
        case 0x7:
            X = THIRD_BIT(ins);
            c->V[X] += FIRST_BYTE(ins);
            break;
        case 0x8:
            X = THIRD_BIT(ins);
            Y = SECOND_BIT(ins);
            switch(FIRST_BIT(ins))
            {
                case 0x0:
                    c->V[X] = c->V[Y];
                    break;
                case 0x1:
                    c->V[X] |= c->V[Y];
                    c->V[0xF] = 0;
                    break;
                case 0x2:
                    c->V[X] &= c->V[Y];
                    c->V[0xF] = 0;
                    break;
                case 0x3:
                    c->V[X] ^= c->V[Y];
                    c->V[0xF] = 0;
                    break;
                case 0x4:
                    if ((c->V[X] + c->V[Y]) >= BYTE_LIMIT) f = 1;
                    else f = 0;
                    c->V[X] += c->V[Y];
                    c->V[0xF] = f;
                    break;
                case 0x5:
                    if (c->V[X] >= c->V[Y]) f = 1;
                    else f = 0;
                    c->V[X] -= c->V[Y];
                    c->V[0xF] = f;
                    break;
                case 0x6:
                    // Ambiguous
                    if (_8xy6_change_VX) c->V[X] = c->V[Y];
                    N = c->V[X];
                    c->V[0xF] = ((c->V[X] >>= 1) << 1) != N ? 1 : 0;
                    break;
                case 0x7:
                    if (c->V[Y] >= c->V[X]) f = 1;
                    else f = 0;
                    c->V[X] = c->V[Y] - c->V[X];
                    c->V[0xF] = f;
                    break;
                case 0xE:
                    // Ambiguous
                    if (_8xy6_change_VX) c->V[X] = c->V[Y];
                    N = c->V[X];
                    c->V[0xF] = ((c->V[X] <<= 1) >> 1) != N ? 1 : 0;
                    break;
            }
            break;
        case 0x9:
            X = THIRD_BIT(ins);
            Y = SECOND_BIT(ins);
            if (c->V[X] != c->V[Y]) c->PC += 2;
            break;
        case 0xA:
            c->I = TRAILING_BITS(ins);
            break;
        case 0xB:
            // Ambiguous
            if (_bnnn_uses_V0) c->PC = TRAILING_BITS(ins) + c->V[0];
            else
            {
                X = THIRD_BIT(ins);
                c->PC = TRAILING_BITS(ins) + c->V[X];
            }
            break;
        case 0xC:
            X = THIRD_BIT(ins);
            // pseudorandom number between 0x00 (0) and 0xFF (255)
            c->V[X] = (rand() % (0xFF - 0x00 + 1) + 0x00) & FIRST_BYTE(ins);
            break;
        case 0xD:
            c->drawn = true;
            X = THIRD_BIT(ins);
            Y = SECOND_BIT(ins);
            N = FIRST_BIT(ins);
            draw(c, window, surface, c->V[X], c->V[Y], N);
            break;
        case 0xE:
            X = THIRD_BIT(ins);
            N = FIRST_NIBBLE(c->V[X]);
            int scan_code = get_scan_code(N);
            keyboard_state = SDL_GetKeyboardState(NULL);
            switch(FIRST_BYTE(ins))
            {
                case 0x9E:
                    if (keyboard_state[scan_code]) c->PC += 2;
                    break;
                case 0xA1:
                    if (!keyboard_state[scan_code]) c->PC += 2;
                    break;
            }
            break;
        case 0xF:
            X = THIRD_BIT(ins);
            switch(FIRST_BYTE(ins))
            {
                case 0x07:
                    c->V[X] = c->Delay;
                    break;
                case 0x0A:
                    c->V[X] = get_key_down();
                    break;
                case 0x15:
                    c->Delay = c->V[X];
                    break;
                case 0x18:
                    c->Sound = c->V[X];
                    break;
                case 0x1E:
                    c->I += c->V[X];
                    if (c->I + c->V[X] >= MEMORY_LIMIT) c->V[0xF] = 1;
                    // might need to set VF = 0 otherwise, but leave as is for now
                    break;
                case 0x29:
                    N = FIRST_NIBBLE(c->V[X]);
                    c->I = mem[FONT_LOCATION + (BYTES_PER_CHAR * N)];
                    break;
                case 0x33:
                    N = c->V[X];
                    N = HEX_TO_DEC(N);
                    if (N < 100) mem[c->I] = 0;
                    else mem[c->I] = (int)floor(N/100);
                    if (N < 10) mem[c->I + 1] = 0;
                    else mem[c->I + 1] = (int)floor(N/10) % 10;
                    mem[c->I + 2] = N % 10;
                    break;
                case 0x55:
                    // Ambiguous
                    if (_fx55_increment_I)
                    {
                        for (size_t i = 0; i <= X; ++i)
                        {
                            mem[c->I++] = c->V[i];
                        }
                    }
                    else
                    {
                        for (size_t i = 0; i <= X; ++i)
                        {
                            mem[c->I + i] = c->V[i];
                        }
                    }
                    break;
                case 0x65:
                    // Ambiguous
                    if (_fx55_increment_I)
                    {
                        for (size_t i = 0; i <= X; ++i)
                        {
                            c->V[i] = mem[c->I++];
                        }
                    }
                    else
                    {
                        for (size_t i = 0; i <= X; ++i)
                        {
                            c->V[i] = mem[c->I + i];
                        }
                    }
                    break;
            }
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
