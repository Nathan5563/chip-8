#include <stddef.h>

#include "map.h"

int map[2048];

void clear_screen()
{
    for (size_t i = 0; i < 2048; ++i)
    {
        map[i] = 0;
    }
}