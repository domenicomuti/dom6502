#ifndef DOM6502_H
#define DOM6502_H

#define DEBUG 0

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#if DEBUG
#include <ctype.h>
#endif

uint8_t ram[65536];

uint16_t pc = 0;
uint8_t sp = 0xFF;
uint8_t ac;
uint8_t xr;
uint8_t yr;
uint8_t sr = 0x32;

#define S_CARRY    0x01
#define S_ZERO     0x02
#define S_INT_DIS  0x04
#define S_DECIMAL  0x08
#define S_OVERFLOW 0x40
#define S_NEGATIVE 0x80

#define _ND 0
#define ACC 1   // Accumulator
#define IMP 2   // Implied
#define IMM 3   // Immediate
#define ZP_ 4   // Zero Page
#define ZPX 5   // Zero Page, X
#define ZPY 6   // Zero Page, Y
#define AB_ 7   // Absolute
#define ABX 8   // Absolute, X
#define ABY 9   // Absolute, Y
#define REL 10  // Relative
#define IN_ 11  // (Indirect)    -> Absolute Indirect addressing (jmp only)
#define INX 12  // (Indirect, X) -> Zero Page Indexed Indirect addressing
#define INY 13  // (Indirect), Y -> Indirect Indexed addressing

typedef struct instruction {
    void *operation;
    uint8_t bytes;
    uint8_t cycles;
    uint8_t mode;
} instruction;

#if DEBUG
void print_asm(const char *function_name, uint8_t mode, uint8_t bytes, uint8_t *cycles) {
    char function_name_upper[4] = {0};
    for (int i = 0; i < 3; i++) {
        function_name_upper[i] = toupper(function_name[i]);
    }    

    printf("%04X ", pc);
    if (bytes == 1) {
        printf("%02X        ", ram[pc]);
    }
    else if (bytes == 2) {
        printf("%02X %02X     ", ram[pc], ram[pc + 1]);
    }
    else if (bytes == 3) {
        printf("%02X %02X %02X  ", ram[pc], ram[pc + 1], ram[pc + 2]);
    }

    printf("%s ", function_name_upper);

    if (mode == IMM) {
        printf("#$%02X      ", ram[pc + 1]);
    }
    else if (mode == ZP_) {
        printf("$%02X       ", ram[pc + 1]);
    }
    else if (mode == ZPX) {
        printf("$%02X,X     ", ram[pc + 1]);
    }
    else if (mode == ZPY) {
        printf("$%02X,Y     ", ram[pc + 1]);
    }
    else if (mode == AB_) {
        printf("$%04X     ", (ram[pc + 2] << 8) | (ram[pc + 1]));
    }
    else if (mode == ABX) {
        printf("$%04X,X   ", (ram[pc + 2] << 8) | (ram[pc + 1]));
    }
    else if (mode == ABY) {
        printf("$%04X,Y   ", (ram[pc + 2] << 8) | (ram[pc + 1]));
    }
    else if (mode == REL) {
        printf("$%04X     ", pc + bytes + (int8_t)ram[pc + 1]);
    }
    else if (mode == INX) {
        printf("($%02X,X)   ", ram[pc + 1]);
    }
    else if (mode == INY) {
        printf("($%02X),Y   ", ram[pc + 1]);
    }
    else {
        printf("          ");
    }

    printf(
        "|%02X %02X %02X %02X %02X|%d%d%d%d%d%d|%d\n", 
        ac, xr, yr, sp, sr,
        ((sr & S_NEGATIVE) >> 7),
        ((sr & S_OVERFLOW) >> 6),
        ((sr & S_DECIMAL) >> 3),
        ((sr & S_INT_DIS) >> 2),
        ((sr & S_ZERO) >> 1),
        (sr & S_CARRY),
        *cycles
    );
}
#endif

void handle_addressing(uint8_t mode, uint8_t **operand, uint8_t *cycles) {
    if (mode == ACC) {
        *operand = &ac;
    }
    else if (mode == IMM) {
        /*  Immediate addressing:
            the operand is contained in the second byte of the instruction.  */
        *operand = &ram[pc + 1];
    }
    else if (mode == ZP_) {
        /*  Zero Page addressing:
            allows shorter code by only fetching the second byte of the instruction
            and assuming a zero high address byte.  */
        *operand = &ram[ram[pc + 1]];
    }
    else if (mode == ZPX) {
        /*  Zero Page Indexed addressing, X:
            the effective address is calculated by adding the second byte to
            the content of the X register. The content of second byte reference
            a location in page zero.  */
        *operand = &ram[ram[pc + 1] + xr];
    }
    else if (mode == ZPY) {
        /*  Zero Page Indexed addressing, Y:
            the effective address is calculated by adding the second byte to
            the contents of the Y register. The content of second byte reference
            a location in page zero.  */
        *operand = &ram[ram[pc + 1] + yr];
    }
    else if (mode == AB_) {
        /*  Absolute addressing:
            the second byte of the instruction specifies the eight low-order bits
            of the effective address, while the third byte specifies the eight
            high-order bits.  */
        *operand = &ram[(ram[pc + 2] << 8) | (ram[pc + 1])];
    }
    else if (mode == ABX) {
        /*  Absolute Indexed addressing, X:
            the effective address is formed by adding the contents of X to the
            address contained in the second and third bytes of the instruction.  */
        uint16_t abs = (ram[pc + 2] << 8) | (ram[pc + 1]);
        uint16_t abs_i = abs + xr;
        if ((abs_i >> 8) != (abs >> 8)) (*cycles)++;
        *operand = &ram[abs_i];
    }
    else if (mode == ABY) {
        /*  Absolute Indexed addressing, Y:
            the effective address is formed by adding the contents of X to the
            address contained in the second and third bytes of the instruction.  */
        uint16_t abs = (ram[pc + 2] << 8) | (ram[pc + 1]);
        uint16_t abs_i = abs + yr;
        if ((abs_i >> 8) != (abs >> 8)) (*cycles)++;
        *operand = &ram[abs_i];
    }
    else if (mode == INX) {
        /*  Zero Page Indexed Indirect addressing (Indirect X):
            the second byte of the instruction is added to the contents of the X
            register; the carry is discarded. The result of this addition points to
            a memory location on page zero whose contents is the low-order eight bits
            of the effective address. The next location in page zero contains the eight
            high-order bits of the effective address. Both memory location must be in
            page zero.  */
        uint8_t p = ram[pc + 1] + xr;
        *operand = &ram[(ram[p + 1] << 8) | ram[p]];
    }
    else if (mode == INY) {
        /*  Indirect Indexed addressing (Indirect Y):
            the second byte of the instruction points to a memory location in page zero.
            The contents of this memory location are added to the contents of Y, the
            result being the low-order eight bits of the effective address.
            The carry from this location is added to the contents of the next page zero
            memory location, the result being the high order eight bits of the effective
            address.  */
        uint8_t z = ram[pc + 1];
        uint16_t abs = (ram[z + 1] << 8) | ram[z];
        uint16_t abs_i = abs + yr;
        if ((abs_i >> 8) != (abs >> 8)) (*cycles)++;
        *operand = &ram[abs_i];
    }
}

void handle_relative(uint8_t *cycles) {
    /*  Relative addressing:
        the second byte of the instruction becomes the operand which is an "Offset"
        added to the contents of the lower eight bits of the program counter when
        the counter is set at the next instruction. The range of the offset is
        -128 to +127 bytes from the next instruction.  */
    (*cycles)++;
    uint16_t _pc = pc + (int8_t)(ram[pc - 1]);
    if ((pc >> 8) != (_pc >> 8)) (*cycles)++;
    pc = _pc;
}

void nul(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void adc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Add with Carry
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t ac_orig = ac;
    uint8_t carry_orig = (sr & S_CARRY);
    uint16_t binary_operation = ac + *operand + carry_orig;

    if (sr & S_DECIMAL) {
        // It works with undocumented behaviours: http://www.6502.org/tutorials/decimal_mode.html
        // TODO: check program in Appendix B

        {
            if (((uint8_t)binary_operation) == 0)
                sr |= S_ZERO;
            else sr &= ~S_ZERO;

            uint8_t t = (ac & 0x0F) + (*operand & 0x0F) + carry_orig;
            if (t >= 0x0A)
                t = ((t + 0x06) & 0x0F) + 0x10;
            uint16_t _ac = (ac & 0xF0) + (*operand & 0xF0) + t;

            if (_ac >= 0xA0)
                _ac += 0x60;
            ac = (uint8_t)_ac;

            if (_ac >= 0x100)
                sr |= S_CARRY;
            else sr &= ~S_CARRY;   
        }
        {
            uint8_t _ac = ac_orig;
            int8_t t = (_ac & 0x0F) + (*operand & 0x0F) + carry_orig;
            if (t >= 0x0A)
                t = ((t + 0x06) & 0x0F) + 0x10;
            _ac = (_ac & 0xF0) + (*operand & 0xF0) + t;

            if (_ac & 0x80)
                sr |= S_NEGATIVE;
            else sr &= ~S_NEGATIVE;

            if ((_ac < -128) | (_ac > 127))
                sr |= S_OVERFLOW;
            else sr &= ~S_OVERFLOW;
        }
    }
    else {
        ac = (uint8_t)binary_operation;

        /*  positive + positive = negative OR 
            negative + negative = positive  */
        bool overflow = (
            (ac_orig >> 7) &&
            (*operand >> 7) &&
            ((ac >> 7) == 0)
        ) ||
        (
            ((ac_orig >> 7) == 0) &&
            ((*operand >> 7) == 0) &&
            (ac >> 7)
        );

        if (overflow)
            sr |= S_OVERFLOW;
        else sr &= ~S_OVERFLOW;

        if (ac >> 7)
            sr |= S_NEGATIVE;
        else sr &= ~S_NEGATIVE;

        if (ac == 0)
            sr |= S_ZERO;
        else sr &= ~S_ZERO;

        if (binary_operation > 0xFF)
            sr |= S_CARRY;
        else sr &= ~S_CARRY;
    }

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
    
	pc += bytes;
}

void and(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Bitwise AND
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    ac &= *operand;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void asl(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Arithmetic Shift Left
	// Modes: ACC, ZP_, ZPX, AB_, ABX
        
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    if (*operand >> 7)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    *operand = *operand << 1;

    if (*operand >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
    
	pc += bytes;
}

void bcc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Carry Clear
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed

	pc += bytes;
    if ((sr & S_CARRY) == 0)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void bcs(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Carry Set
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed
	
    pc += bytes;
    if (sr & S_CARRY)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void beq(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Equal
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed

	pc += bytes;
    if (sr & S_ZERO)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void bit(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Bit Test
	// Modes: ZP_, AB_
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t _ac = ac;
    _ac &= *operand;

    if (_ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (_ac & 0x40)
        sr |= S_OVERFLOW;
    else sr &= ~S_OVERFLOW;

    if (_ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void bmi(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Minus
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed

	pc += bytes;
    if (sr & S_NEGATIVE)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void bne(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Not Equal
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed

	pc += bytes;
    if ((sr & S_ZERO) == 0)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void bpl(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Plus
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed

	pc += bytes;
    if ((sr & S_NEGATIVE) == 0)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void brk(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Break (software IRQ)
	// Modes: IMP

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void bvc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Overflow Clear
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed
	
    pc += bytes;
    if ((sr & S_OVERFLOW) == 0)
        handle_relative(cycles);
        
    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void bvs(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Branch if Overflow Set
	// Modes: REL
	// REL: 3 cycles if branch taken, 4 cycles if page crossed
	
    pc += bytes;
    if (sr & S_OVERFLOW)
        handle_relative(cycles);

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
}

void clc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Clear Carry
	// Modes: IMP

    sr &= ~S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void cld(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Clear Decimal Mode
	// Modes: IMP

    sr &= ~S_DECIMAL;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void cli(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Clear Interrupt Disable
	// Modes: IMP

    sr &= ~S_INT_DIS;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void clv(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Clear Overflow
	// Modes: IMP

    sr &= ~S_OVERFLOW;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void cmp(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Compare A
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t operation = ac - *operand;

    if (operation == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (operation >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand <= ac)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void cpx(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Compare X
	// Modes: IMM, ZP_, AB_
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    int8_t operation = xr - *operand;

    if (operation == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (operation >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (operation >= 0)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void cpy(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Compare Y
	// Modes: IMM, ZP_, AB_
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    int8_t operation = yr - *operand;

    if (operation == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (operation >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (operation >= 0)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void dec(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Decrement Memory
	// Modes: ZP_, ZPX, AB_, ABX

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    
    (*operand)--;

    if (*operand >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void dex(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Decrement X
	// Modes: IMP

    xr--;

    if (xr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (xr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void dey(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Decrement Y
	// Modes: IMP

    yr--;

    if (yr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (yr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
    
	pc += bytes;
}

void eor(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Bitwise Exclusive OR
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    ac ^= *operand;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void inc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Increment Memory
	// Modes: ZP_, ZPX, AB_, ABX
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    
    (*operand)++;

    if (*operand >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;
    
    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void inx(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Increment X
	// Modes: IMP
    
    xr++;

    if (xr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (xr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void iny(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Increment Y
	// Modes: IMP

    yr++;

    if (yr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (yr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void jmp(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Jump
	// Modes: AB_, IN_

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    if (mode == IN_) {
        /*  Absolute Indirect addressing:
            the second byte of the instruction contains the low-order eight bits of a
            memory location. The high-order eight bits of that memory location is
            contained in the third byte of the instruction.
            The contents of the fully specified memory location is the low-order byte
            of the effective address. The next memory location contains the high-order
            eight bytes of the effective address which is loaded into the 16 bit
            program counter.  */
        uint16_t p = (ram[pc + 2] << 8) | (ram[pc + 1]);
        pc = ((ram[p + 1] << 8) | ram[p]);
    }
    else pc = (ram[pc + 2] << 8) | (ram[pc + 1]);
}

void jsr(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Jump to Subroutine
	// Modes: AB_

    uint16_t _pc = pc + 2;
    ram[0x0100 + sp--] = (_pc & 0xFF00) >> 8;
    ram[0x0100 + sp--] = _pc & 0x00FF;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    pc = (ram[pc + 2] << 8) | ram[pc + 1];
}

void lda(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Load A
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    
    ac = *operand;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void ldx(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Load X
	// Modes: IMM, ZP_, ZPY, AB_, ABY
	// ABY: 5 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    
    xr = *operand;

    if (xr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (xr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void ldy(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Load Y
	// Modes: IMM, ZP_, ZPX, AB_, ABX
	// ABX: 5 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    
    yr = *operand;

    if (yr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (yr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void lsr(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Logical Shift Right
	// Modes: ACC, ZP_, ZPX, AB_, ABX

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t carry = *operand & 1;
    *operand = (*operand) >> 1;

    sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (carry)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void nop(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// No Operation
	// Modes: IMP

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void ora(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Bitwise OR
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    ac |= *operand;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void pha(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Push A
	// Modes: IMP

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    ram[0x0100 + sp--] = ac;
	pc += bytes;
}

void php(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Push Processor Status
	// Modes: IMP
	
    ram[0x0100 + sp--] = sr;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void pla(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Pull A
	// Modes: IMP

    sp++;
    ac = ram[0x0100 + sp];

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void plp(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Pull Processor Status
	// Modes: IMP

    sp++;
    sr = ram[0x0100 + sp];

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void rol(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Rotate Left
	// Modes: ACC, ZP_, ZPX, AB_, ABX

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t out_carry = (*operand) >> 7;
    uint8_t in_carry = sr & S_CARRY;
    *operand = ((*operand) << 1) | in_carry;

    if (out_carry)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    if ((*operand) >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void ror(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Rotate Right
	// Modes: ACC, ZP_, ZPX, AB_, ABX
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    uint8_t out_carry = (*operand) & 1;
    uint8_t in_carry = sr & S_CARRY;
    *operand = ((*operand) >> 1) | (in_carry << 7);

    if (out_carry)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    if (in_carry)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (*operand == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void rti(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Return from Interrupt
	// Modes: IMP

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    sr = ram[0x0100 + sp++] & 0xCF;
    pc = (ram[0x0100 + sp + 2] << 8) | ram[0x0100 + sp + 1];
    sp += 2;
}

void rts(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Return from Subroutine
	// Modes: IMP

    sp += 2;

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc = ((ram[0x0100 + sp] << 8) | ram[0x0100 + sp - 1]) + 1;
    
}

void sbc(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Subtract with Carry
	// Modes: IMM, ZP_, ZPX, AB_, ABX, ABY, INX, INY
	// INY: 6 cycles if page crossed

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
    int8_t operand_i = (int8_t)*operand;

    uint8_t carry_orig = (sr & S_CARRY);
    int16_t binary_operation = (int8_t)ac - operand_i - !carry_orig;

    uint8_t ac_bin = (uint8_t)binary_operation;

    if (ac_bin < 0x80)
        sr |= S_CARRY;
    else sr &= ~S_CARRY;

    if (binary_operation < -128 || binary_operation > 127)
        sr |= S_OVERFLOW;
    else sr &= ~S_OVERFLOW;

    if (ac_bin >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac_bin == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    if (sr & S_DECIMAL) {
        int8_t t = (ac & 0x0F) - (operand_i & 0x0F) - !carry_orig;
        if (t < 0)
            t = ((t - 0x06) & 0x0F) - 0x10;
        int16_t _ac = (ac & 0xF0) - (operand_i & 0xF0) + t;
        if (_ac < 0)
            _ac -= 0x60;
        ac = (int8_t)_ac;
    }
    else {
        ac = ac_bin;        
    }

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void sec(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Set Carry
	// Modes: IMP

    sr |= S_CARRY;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void sed(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Set Decimal Mode
	// Modes: IMP

    sr |= S_DECIMAL;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void sei(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Set Interrupt Disable
	// Modes: IMP

    sr |= S_INT_DIS;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
    
	pc += bytes;
}

void sta(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Store A
	// Modes: ZP_, ZPX, AB_, ABX, ABY, INX, INY

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    *operand = ac;
	pc += bytes;
}

void stx(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Store X
	// Modes: ZP_, ZPY, AB_

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif
	
    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);

    *operand = xr;
	pc += bytes;
}

void sty(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Store Y
	// Modes: ZP_, ZPX, AB_

	#if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

    uint8_t *operand = NULL;
    handle_addressing(mode, &operand, cycles);
	
    *operand = yr;
	pc += bytes;
}

void tax(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer A to X
	// Modes: IMP

    xr = ac;

    if (xr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (xr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void tay(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer A to Y
	// Modes: IMP
	
    yr = ac;
    
    if (yr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (yr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void tsx(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer Stack Pointer to X
	// Modes: IMP

    xr = sp;
    
    if (xr >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (xr == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void txa(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer X to A
	// Modes: IMP
	
    ac = xr;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void txs(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer X to Stack Pointer
	// Modes: IMP

    sp = xr;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

void tya(uint8_t bytes, uint8_t *cycles, uint8_t mode) {
	// Transfer Y to A
	// Modes: IMP
	
    ac = yr;

    if (ac >> 7)
        sr |= S_NEGATIVE;
    else sr &= ~S_NEGATIVE;

    if (ac == 0)
        sr |= S_ZERO;
    else sr &= ~S_ZERO;

    #if DEBUG
	print_asm(__func__, mode, bytes, cycles);
	#endif

	pc += bytes;
}

instruction instructions[] = {
    {brk, 1, 7, IMP},   // 0x00
    {ora, 2, 6, INX},   // 0x01
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ora, 2, 3, ZP_},   // 0x05
    {asl, 2, 5, ZP_},   // 0x06
    {nul, 0, 0, _ND},
    {php, 1, 3, IMP},   // 0x08
    {ora, 2, 2, IMM},   // 0x09
    {asl, 1, 2, ACC},   // 0x0A
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ora, 3, 4, AB_},   // 0x0D
    {asl, 3, 6, AB_},   // 0x0E
    {nul, 0, 0, _ND},
    {bpl, 2, 2, REL},   // 0x10
    {ora, 2, 5, INY},   // 0x11
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ora, 2, 4, ZPX},   // 0x15
    {asl, 2, 6, ZPX},   // 0x16
    {nul, 0, 0, _ND},
    {clc, 1, 2, IMP},   // 0x18
    {ora, 3, 4, ABY},   // 0x19
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ora, 3, 4, ABX},   // 0x1D
    {asl, 3, 7, ABX},   // 0x1E
    {nul, 0, 0, _ND},
    {jsr, 3, 6, AB_},   // 0x20
    {and, 2, 6, INX},   // 0x21
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {bit, 2, 3, ZP_},   // 0x24
    {and, 2, 3, ZP_},   // 0x25
    {rol, 2, 5, ZP_},   // 0x26
    {nul, 0, 0, _ND},
    {plp, 1, 4, IMP},   // 0x28
    {and, 2, 2, IMM},   // 0x29
    {rol, 1, 2, ACC},   // 0x2A
    {nul, 0, 0, _ND},
    {bit, 3, 4, AB_},   // 0x2C
    {and, 3, 4, AB_},   // 0x2D
    {rol, 3, 6, AB_},   // 0x2E
    {nul, 0, 0, _ND},
    {bmi, 2, 2, REL},   // 0x30
    {and, 2, 5, INY},   // 0x31
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {and, 2, 4, ZPX},   // 0x35
    {rol, 2, 6, ZPX},   // 0x36
    {nul, 0, 0, _ND},
    {sec, 1, 2, IMP},   // 0x38
    {and, 3, 4, ABY},   // 0x39
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {and, 3, 4, ABX},   // 0x3D
    {rol, 3, 7, ABX},   // 0x3E
    {nul, 0, 0, _ND},
    {rti, 1, 6, IMP},   // 0x40
    {eor, 2, 6, INX},   // 0x41
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {eor, 2, 3, ZP_},   // 0x45
    {lsr, 2, 5, ZP_},   // 0x46
    {nul, 0, 0, _ND},
    {pha, 1, 3, IMP},   // 0x48
    {eor, 2, 2, IMM},   // 0x49
    {lsr, 1, 2, ACC},   // 0x4A
    {nul, 0, 0, _ND},
    {jmp, 3, 3, AB_},   // 0x4C
    {eor, 3, 4, AB_},   // 0x4D
    {lsr, 3, 6, AB_},   // 0x4E
    {nul, 0, 0, _ND},
    {bvc, 2, 2, REL},   // 0x50
    {eor, 2, 5, INY},   // 0x51
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {eor, 2, 4, ZPX},   // 0x55
    {lsr, 2, 6, ZPX},   // 0x56
    {nul, 0, 0, _ND},
    {cli, 1, 2, IMP},   // 0x58
    {eor, 3, 4, ABY},   // 0x59
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {eor, 3, 4, ABX},   // 0x5D
    {lsr, 3, 7, ABX},   // 0x5E
    {nul, 0, 0, _ND},
    {rts, 1, 6, IMP},   // 0x60
    {adc, 2, 6, INX},   // 0x61
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {adc, 2, 3, ZP_},   // 0x65
    {ror, 2, 5, ZP_},   // 0x66
    {nul, 0, 0, _ND},
    {pla, 1, 4, IMP},   // 0x68
    {adc, 2, 2, IMM},   // 0x69
    {ror, 1, 2, ACC},   // 0x6A
    {nul, 0, 0, _ND},
    {jmp, 3, 5, IN_},   // 0x6C
    {adc, 3, 4, AB_},   // 0x6D
    {ror, 3, 6, AB_},   // 0x6E
    {nul, 0, 0, _ND},
    {bvs, 2, 2, REL},   // 0x70
    {adc, 2, 5, INY},   // 0x71
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {adc, 2, 4, ZPX},   // 0x75
    {ror, 2, 6, ZPX},   // 0x76
    {nul, 0, 0, _ND},
    {sei, 1, 2, IMP},   // 0x78
    {adc, 3, 4, ABY},   // 0x79
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {adc, 3, 4, ABX},   // 0x7D
    {ror, 3, 7, ABX},   // 0x7E
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sta, 2, 6, INX},   // 0x81
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sty, 2, 3, ZP_},   // 0x84
    {sta, 2, 3, ZP_},   // 0x85
    {stx, 2, 3, ZP_},   // 0x86
    {nul, 0, 0, _ND},
    {dey, 1, 2, IMP},   // 0x88
    {nul, 0, 0, _ND},
    {txa, 1, 2, IMP},   // 0x8A
    {nul, 0, 0, _ND},
    {sty, 3, 4, AB_},   // 0x8C
    {sta, 3, 4, AB_},   // 0x8D
    {stx, 3, 4, AB_},   // 0x8E
    {nul, 0, 0, _ND},
    {bcc, 2, 2, REL},   // 0x90
    {sta, 2, 6, INY},   // 0x91
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sty, 2, 4, ZPX},   // 0x94
    {sta, 2, 4, ZPX},   // 0x95
    {stx, 2, 4, ZPY},   // 0x96
    {nul, 0, 0, _ND},
    {tya, 1, 2, IMP},   // 0x98
    {sta, 3, 5, ABY},   // 0x99
    {txs, 1, 2, IMP},   // 0x9A
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sta, 3, 5, ABX},   // 0x9D
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ldy, 2, 2, IMM},   // 0xA0
    {lda, 2, 6, INX},   // 0xA1
    {ldx, 2, 2, IMM},   // 0xA2
    {nul, 0, 0, _ND},
    {ldy, 2, 3, ZP_},   // 0xA4
    {lda, 2, 3, ZP_},   // 0xA5
    {ldx, 2, 3, ZP_},   // 0xA6
    {nul, 0, 0, _ND},
    {tay, 1, 2, IMP},   // 0xA8
    {lda, 2, 2, IMM},   // 0xA9
    {tax, 1, 2, IMP},   // 0xAA
    {nul, 0, 0, _ND},
    {ldy, 3, 4, AB_},   // 0xAC
    {lda, 3, 4, AB_},   // 0xAD
    {ldx, 3, 4, AB_},   // 0xAE
    {nul, 0, 0, _ND},
    {bcs, 2, 2, REL},   // 0xB0
    {lda, 2, 5, INY},   // 0xB1
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {ldy, 2, 4, ZPX},   // 0xB4
    {lda, 2, 4, ZPX},   // 0xB5
    {ldx, 2, 4, ZPY},   // 0xB6
    {nul, 0, 0, _ND},
    {clv, 1, 2, IMP},   // 0xB8
    {lda, 3, 4, ABY},   // 0xB9
    {tsx, 1, 2, IMP},   // 0xBA
    {nul, 0, 0, _ND},
    {ldy, 3, 4, ABX},   // 0xBC
    {lda, 3, 4, ABX},   // 0xBD
    {ldx, 3, 4, ABY},   // 0xBE
    {nul, 0, 0, _ND},
    {cpy, 2, 2, IMM},   // 0xC0
    {cmp, 2, 6, INX},   // 0xC1
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {cpy, 2, 3, ZP_},   // 0xC4
    {cmp, 2, 3, ZP_},   // 0xC5
    {dec, 2, 5, ZP_},   // 0xC6
    {nul, 0, 0, _ND},
    {iny, 1, 2, IMP},   // 0xC8
    {cmp, 2, 2, IMM},   // 0xC9
    {dex, 1, 2, IMP},   // 0xCA
    {nul, 0, 0, _ND},
    {cpy, 3, 4, AB_},   // 0xCC
    {cmp, 3, 4, AB_},   // 0xCD
    {dec, 3, 6, AB_},   // 0xCE
    {nul, 0, 0, _ND},
    {bne, 2, 2, REL},   // 0xD0
    {cmp, 2, 5, INY},   // 0xD1
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {cmp, 2, 4, ZPX},   // 0xD5
    {dec, 2, 6, ZPX},   // 0xD6
    {nul, 0, 0, _ND},
    {cld, 1, 2, IMP},   // 0xD8
    {cmp, 3, 4, ABY},   // 0xD9
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {cmp, 3, 4, ABX},   // 0xDD
    {dec, 3, 7, ABX},   // 0xDE
    {nul, 0, 0, _ND},
    {cpx, 2, 2, IMM},   // 0xE0
    {sbc, 2, 6, INX},   // 0xE1
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {cpx, 2, 3, ZP_},   // 0xE4
    {sbc, 2, 3, ZP_},   // 0xE5
    {inc, 2, 5, ZP_},   // 0xE6
    {nul, 0, 0, _ND},
    {inx, 1, 2, IMP},   // 0xE8
    {sbc, 2, 2, IMM},   // 0xE9
    {nop, 1, 2, IMP},   // 0xEA
    {nul, 0, 0, _ND},
    {cpx, 3, 4, AB_},   // 0xEC
    {sbc, 3, 4, AB_},   // 0xED
    {inc, 3, 6, AB_},   // 0xEE
    {nul, 0, 0, _ND},
    {beq, 2, 2, REL},   // 0xF0
    {sbc, 2, 5, INY},   // 0xF1
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sbc, 2, 4, ZPX},   // 0xF5
    {inc, 2, 6, ZPX},   // 0xF6
    {nul, 0, 0, _ND},
    {sed, 1, 2, IMP},   // 0xF8
    {sbc, 3, 4, ABY},   // 0xF9
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {nul, 0, 0, _ND},
    {sbc, 3, 4, ABX},   // 0xFD
    {inc, 3, 7, ABX}    // 0xFE
};

#endif