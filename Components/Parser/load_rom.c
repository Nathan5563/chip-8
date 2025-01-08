#include <stdio.h>
#include <stdlib.h>

#include "load_rom.h"
#include "../Memory/mem.h"

void load_rom(FILE* rom)
{
    fseek(rom, 0, SEEK_END);
    size_t file_size = ftell(rom);
    rewind(rom);

    BYTE* buffer = malloc(file_size);
    // TODO: handle memory allocation failure
    fread(buffer, 1, file_size, rom);

    for (size_t i = 0; i < file_size; ++i)
    {
        mem[i + 0x200] = buffer[i];
    }

    free(buffer);
}