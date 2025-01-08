#include <stdlib.h>

#include "cpu.h"
#include "../Stack/stack.h"
#include "../Map/map.h"
#include "../font.h"
#include "../Memory/mem.h"

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
    c->PC = 0;
    c->I = 0;
    c->Delay = 0;
    c->Sound = 0;
    for (size_t i = 0; i < 4096; ++i)
    {
        if (i < FONT_SIZE) mem[i + 80] = font[i];
        if (!(i >= 80 && i < 160)) mem[i] = 0;
        if (i < 16) c->V[i] = 0;
        if (i < 2048) map[i] = 0;
    }
    return c;
}

void cpu_destroy(cpu* c)
{
    stack_destroy(c->Stack);
    free(c);
}