#include <stdio.h>

#include "read_rom.h"

unsigned char* read_rom(FILE* rom)
{
    fseek(rom, 0, SEEK_END); 
    size_t file_size = ftell(rom);
    rewind(rom);

    unsigned char* buffer = malloc(file_size);
    // TODO: handle memory allocation failure
    fread(buffer, 1, file_size, rom);

    return buffer;
}