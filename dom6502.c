#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "dom6502.h"

uint16_t start_program = 0xC000;

int main() {
    struct sched_param _sched_param;
    _sched_param.sched_priority = 99;
    sched_setscheduler(0, SCHED_FIFO, &_sched_param);

    FILE* fptr = fopen("1541rom.bin", "rb");
    fseek(fptr, 0L, SEEK_END);
    int filesize = ftell(fptr);
    rewind(fptr);

    fread(ram + start_program, filesize, 1, fptr);
    fclose(fptr);

    pc = (ram[0xFFFD] << 8) | ram[0xFFFC];

    uint8_t *b;
    do {
        suseconds_t a = get_microsec();
        
        b = ram + pc;
        instruction i = instructions[*b];
        void (*func)() = i.operation;
        func(i.bytes, &i.cycles, i.mode);

        microsleep((i.cycles / SPEED) - (get_microsec() - a));

        if (irq && ((sr & S_INT_DIS) == 0)) {
            irq = false;
            ram[0x0100 + sp--] = pc >> 8;
            ram[0x0100 + sp--] = pc & 0x00FF;
            ram[0x0100 + sp--] = sr;
            pc = (ram[0xFFFF] << 8) | ram[0xFFFE];
        }
    } while (*b != 0);

    return 0;
}

