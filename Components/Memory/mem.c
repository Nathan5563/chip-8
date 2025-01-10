#include "mem.h"
#include <stddef.h>

BYTE mem[4096];

void clear_mem()
{
    for (size_t i = 0; i < 4096; ++i)
    {
        mem[i] = 0x00;
    }
}