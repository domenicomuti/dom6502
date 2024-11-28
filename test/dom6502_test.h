#ifndef DOM6502_TEST_H
#define DOM6502_TEST_H

#include <stdio.h>
#include "../dom6502.h"

#define COLOR_RESET "\x1B[0m"
#define COLOR_RED   "\x1B[31m"
#define COLOR_GREEN "\x1B[32m"

uint16_t start_program = 0xC000;

void write_ram(uint8_t bytes, uint16_t operand) {
    if (bytes == 3) {
        ram[pc + 1] = operand & 0x00FF;
        ram[pc + 2] = (operand & 0xFF00) >> 8;
    }
    else {
        ram[pc + 1] = operand;
    }
}

void a_brk() {
	ram[pc++] = 0x00;
}

void a_ora(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0x01;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0x05;
			bytes = 2;
			break;
		case IMM:
			opcode = 0x09;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x0D;
			bytes = 3;
			break;
		case INY:
			opcode = 0x11;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0x15;
			bytes = 2;
			break;
		case ABY:
			opcode = 0x19;
			bytes = 3;
			break;
		case ABX:
			opcode = 0x1D;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_asl(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x06;
			bytes = 2;
			break;
		case ACC:
			opcode = 0x0A;
			bytes = 1;
			break;
		case AB_:
			opcode = 0x0E;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0x16;
			bytes = 2;
			break;
		case ABX:
			opcode = 0x1E;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_php() {
	ram[pc++] = 0x08;
}

void a_bpl(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0x10;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_clc() {
	ram[pc++] = 0x18;
}

void a_jsr(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case AB_:
			opcode = 0x20;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_and(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0x21;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0x25;
			bytes = 2;
			break;
		case IMM:
			opcode = 0x29;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x2D;
			bytes = 3;
			break;
		case INY:
			opcode = 0x31;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0x35;
			bytes = 2;
			break;
		case ABY:
			opcode = 0x39;
			bytes = 3;
			break;
		case ABX:
			opcode = 0x3D;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_bit(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x24;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x2C;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_rol(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x26;
			bytes = 2;
			break;
		case ACC:
			opcode = 0x2A;
			bytes = 1;
			break;
		case AB_:
			opcode = 0x2E;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0x36;
			bytes = 2;
			break;
		case ABX:
			opcode = 0x3E;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_plp() {
	ram[pc++] = 0x28;
}

void a_bmi(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0x30;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_sec() {
	ram[pc++] = 0x38;
}

void a_rti() {
	ram[pc++] = 0x40;
}

void a_eor(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0x41;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0x45;
			bytes = 2;
			break;
		case IMM:
			opcode = 0x49;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x4D;
			bytes = 3;
			break;
		case INY:
			opcode = 0x51;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0x55;
			bytes = 2;
			break;
		case ABY:
			opcode = 0x59;
			bytes = 3;
			break;
		case ABX:
			opcode = 0x5D;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_lsr(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x46;
			bytes = 2;
			break;
		case ACC:
			opcode = 0x4A;
			bytes = 1;
			break;
		case AB_:
			opcode = 0x4E;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0x56;
			bytes = 2;
			break;
		case ABX:
			opcode = 0x5E;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_pha() {
	ram[pc++] = 0x48;
}

void a_jmp(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case AB_:
			opcode = 0x4C;
			bytes = 3;
			break;
		case IN_:
			opcode = 0x6C;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_bvc(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0x50;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_cli() {
	ram[pc++] = 0x58;
}

void a_rts() {
	ram[pc++] = 0x60;
}

void a_adc(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0x61;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0x65;
			bytes = 2;
			break;
		case IMM:
			opcode = 0x69;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x6D;
			bytes = 3;
			break;
		case INY:
			opcode = 0x71;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0x75;
			bytes = 2;
			break;
		case ABY:
			opcode = 0x79;
			bytes = 3;
			break;
		case ABX:
			opcode = 0x7D;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_ror(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x66;
			bytes = 2;
			break;
		case ACC:
			opcode = 0x6A;
			bytes = 1;
			break;
		case AB_:
			opcode = 0x6E;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0x76;
			bytes = 2;
			break;
		case ABX:
			opcode = 0x7E;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_pla() {
	ram[pc++] = 0x68;
}

void a_bvs(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0x70;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_sei() {
	ram[pc++] = 0x78;
}

void a_sta(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0x81;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0x85;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x8D;
			bytes = 3;
			break;
		case INY:
			opcode = 0x91;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0x95;
			bytes = 2;
			break;
		case ABY:
			opcode = 0x99;
			bytes = 3;
			break;
		case ABX:
			opcode = 0x9D;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_sty(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x84;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x8C;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0x94;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_stx(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0x86;
			bytes = 2;
			break;
		case AB_:
			opcode = 0x8E;
			bytes = 3;
			break;
		case ZPY:
			opcode = 0x96;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_dey() {
	ram[pc++] = 0x88;
}

void a_txa() {
	ram[pc++] = 0x8A;
}

void a_bcc(uint16_t operand) {
	uint8_t bytes = 2;
	ram[pc] = 0x90;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_tya() {
	ram[pc++] = 0x98;
}

void a_txs() {
	ram[pc++] = 0x9A;
}

void a_ldy(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case IMM:
			opcode = 0xA0;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xA4;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xAC;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0xB4;
			bytes = 2;
			break;
		case ABX:
			opcode = 0xBC;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_lda(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0xA1;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xA5;
			bytes = 2;
			break;
		case IMM:
			opcode = 0xA9;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xAD;
			bytes = 3;
			break;
		case INY:
			opcode = 0xB1;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0xB5;
			bytes = 2;
			break;
		case ABY:
			opcode = 0xB9;
			bytes = 3;
			break;
		case ABX:
			opcode = 0xBD;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_ldx(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case IMM:
			opcode = 0xA2;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xA6;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xAE;
			bytes = 3;
			break;
		case ZPY:
			opcode = 0xB6;
			bytes = 2;
			break;
		case ABY:
			opcode = 0xBE;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_tay() {
	ram[pc++] = 0xA8;
}

void a_tax() {
	ram[pc++] = 0xAA;
}

void a_bcs(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0xB0;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_clv() {
	ram[pc++] = 0xB8;
}

void a_tsx() {
	ram[pc++] = 0xBA;
}

void a_cpy(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case IMM:
			opcode = 0xC0;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xC4;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xCC;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_cmp(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0xC1;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xC5;
			bytes = 2;
			break;
		case IMM:
			opcode = 0xC9;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xCD;
			bytes = 3;
			break;
		case INY:
			opcode = 0xD1;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0xD5;
			bytes = 2;
			break;
		case ABY:
			opcode = 0xD9;
			bytes = 3;
			break;
		case ABX:
			opcode = 0xDD;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_dec(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0xC6;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xCE;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0xD6;
			bytes = 2;
			break;
		case ABX:
			opcode = 0xDE;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_iny() {
	ram[pc++] = 0xC8;
}

void a_dex() {
	ram[pc++] = 0xCA;
}

void a_bne(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0xD0;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_cld() {
	ram[pc++] = 0xD8;
}

void a_cpx(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case IMM:
			opcode = 0xE0;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xE4;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xEC;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_sbc(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case INX:
			opcode = 0xE1;
			bytes = 2;
			break;
		case ZP_:
			opcode = 0xE5;
			bytes = 2;
			break;
		case IMM:
			opcode = 0xE9;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xED;
			bytes = 3;
			break;
		case INY:
			opcode = 0xF1;
			bytes = 2;
			break;
		case ZPX:
			opcode = 0xF5;
			bytes = 2;
			break;
		case ABY:
			opcode = 0xF9;
			bytes = 3;
			break;
		case ABX:
			opcode = 0xFD;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_inc(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case ZP_:
			opcode = 0xE6;
			bytes = 2;
			break;
		case AB_:
			opcode = 0xEE;
			bytes = 3;
			break;
		case ZPX:
			opcode = 0xF6;
			bytes = 2;
			break;
		case ABX:
			opcode = 0xFE;
			bytes = 3;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_inx() {
	ram[pc++] = 0xE8;
}

void a_nop() {
	ram[pc++] = 0xEA;
}

void a_beq(uint16_t operand, uint8_t mode) {
	uint8_t opcode;
	uint8_t bytes;
	switch (mode) {
		case REL:
			opcode = 0xF0;
			bytes = 2;
			break;
		default:
			printf("%sERROR %s%s\n", COLOR_RED, __func__, COLOR_RESET);
			return;
	}
	ram[pc] = opcode;
	write_ram(bytes, operand);
	pc += bytes;
}

void a_sed() {
	ram[pc++] = 0xF8;
}

void reset_pc() {
    pc = start_program;
}

void reset_status() {
	sr = 0x32;
}

void run_6502() {
    reset_pc();
	uint8_t *b;
    do {
        b = ram + pc;
        instruction i = instructions[*b];
        void (*func)() = i.operation;
        func(i.bytes, i.cycles, i.mode);
	} while (*b != 0);
}

void assert_reg_equals(uint8_t *reg, uint8_t value, char *test_name) {
    printf("%s ", test_name);
    if (*reg == value)
        printf("%sTEST OK", COLOR_GREEN);
    else
        printf("%sTEST NOK", COLOR_RED);
    printf("%s\n", COLOR_RESET);
}

#endif