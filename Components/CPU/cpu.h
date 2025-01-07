#ifndef __CPU_H__
#define __CPU_H__

typedef unsigned char BYTE;
typedef unsigned short WORD;

BYTE mem[4096];

struct _cpu;
typedef struct _cpu cpu;

cpu* cpu_init();

void cpu_emulate_cycle();

WORD cpu_fetch(cpu* c);

void cpu_execute(cpu* c, WORD ins);

#endif