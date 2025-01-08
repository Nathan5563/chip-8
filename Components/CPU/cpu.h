#ifndef __CPU_H__
#define __CPU_H__

#include "../types.h"

struct _cpu;
typedef struct _cpu cpu;

cpu* cpu_init();

void cpu_emulate_cycle();

WORD cpu_fetch(cpu* c);

void cpu_execute(cpu* c, WORD ins);

void cpu_destroy(cpu* c);

#endif