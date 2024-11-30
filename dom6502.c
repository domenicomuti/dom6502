#include <stdio.h>
#include <stdlib.h>
#include "dom6502.h"

uint16_t start_program = 0xC000;

int main(uint8_t bytes, uint8_t cycles, uint8_t mode) {
    FILE* fptr = fopen("1541rom.bin", "rb");
    fseek(fptr, 0L, SEEK_END);
    int filesize = ftell(fptr);
    rewind(fptr);

    fread(ram + start_program, filesize, 1, fptr);
    fclose(fptr);
    
    //pc = 0xE85B;
    pc = 0xCAB2;

    while (pc < 65536) {
        uint8_t *b = ram + pc;

        instruction i = instructions[*b];

        void (*func)() = i.operation;
        func(i.bytes, &i.cycles, i.mode);
    }

    return 0;
}

