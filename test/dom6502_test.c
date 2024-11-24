#include "dom6502_test.h"

int main() {

	// start: lda, ldx, ldy
    reset_pc();
    a_lda(0x11, IMM);
    run_6502();
    assert_reg_equals(&ac, 0x11, "lda immediate mode");

	reset_pc();
	ram[0] = 0x12;
	a_lda(0, ZP_);
	run_6502();
	assert_reg_equals(&ac, 0x12, "lda zero page mode");

	reset_pc();
	ram[3] = 0x13;
	a_ldx(1, IMM);
	a_lda(2, ZPX);
	run_6502();
	assert_reg_equals(&ac, 0x13, "lda zero page indexed X mode");

	reset_pc();
	ram[0xFFFE] = 0x14;
	a_lda(0xFFFE, AB_);
	run_6502();
	assert_reg_equals(&ac, 0x14, "lda absolute mode");

	reset_pc();
	ram[0xFFFE] = 0x15;
	a_ldx(1, IMM);
	a_lda(0xFFFD, ABX);
	run_6502();
	assert_reg_equals(&ac, 0x15, "lda absolute mode indexed X");

	reset_pc();
	ram[0xFFFE] = 0x16;
	a_ldy(1, IMM);
	a_lda(0xFFFD, ABY);
	run_6502();
	assert_reg_equals(&ac, 0x16, "lda absolute mode indexed Y");

	reset_pc();
	ram[0x00AA] = 0x12;
	ram[0x00AB] = 0x13;
	ram[0x1312] = 0x17;
	a_ldx(0xA9, IMM);
	a_lda(1, INX);
	run_6502();
	assert_reg_equals(&ac, 0x17, "lda zero page indexed indirect addressing (indirect x) [1]");

	reset_pc();
	ram[0x0000] = 0x12;
	ram[0x0001] = 0x13;
	ram[0x1312] = 0x18;
	a_ldx(0xFF, IMM);
	a_lda(1, INX);
	run_6502();
	assert_reg_equals(&ac, 0x18, "lda zero page indexed indirect addressing (indirect x) [2]");

	reset_pc();
	ram[0x0003] = 0xFF;
	ram[0x0004] = 0x14;
	ram[0x1500] = 0x19;
	a_ldy(1, IMM);
	a_lda(3, INY);
	run_6502();
	assert_reg_equals(&ac, 0x19, "lda indirect indexed addressing (indirect y)");
	// end: lda, ldx, ldy


	// start: clc, sec, adc, sbc
	reset_pc();
	reset_status();
	a_clc();
	run_6502();
	assert_reg_equals(&sr, 0x32, "clc");

	reset_pc();
	reset_status();
	a_sec();
	run_6502();
	assert_reg_equals(&sr, 0x33, "sec");

	reset_pc();
	reset_status();
	a_lda(0x01, IMM);
	a_adc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 2, "adc [1a]");
	assert_reg_equals(&sr, 0x30, "adc [1b]");

	reset_pc();
	reset_status();
	a_lda(0x01, IMM);
	a_adc(0xFF, IMM);
	run_6502();
	assert_reg_equals(&ac, 0, "adc [2a]");
	assert_reg_equals(&sr, 0x33, "adc [2b]");

	reset_pc();
	reset_status();
	a_lda(0x7F, IMM);
	a_adc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x80, "adc [3a]");
	assert_reg_equals(&sr, 0xF0, "adc [3b]");

	reset_pc();
	reset_status();
	a_lda(0x80, IMM);
	a_adc(0xFF, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x7F, "adc [4a]");
	assert_reg_equals(&sr, 0x71, "adc [4b]");

	reset_pc();
	reset_status();
	a_sec();
	a_lda(0x00, IMM);
	a_sbc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 0xFF, "sbc [1a]");
	assert_reg_equals(&sr, 0xB0, "sbc [1b]");

	reset_pc();
	reset_status();
	a_sec();
	a_lda(0x80, IMM);
	a_sbc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x7F, "sbc [2a]");
	assert_reg_equals(&sr, 0x71, "sbc [2b]");

	reset_pc();
	reset_status();
	a_sec();
	a_lda(0x7F, IMM);
	a_sbc(0xFF, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x80, "sbc [3a]");
	assert_reg_equals(&sr, 0xF0, "sbc [3b]");

	reset_pc();
	reset_status();
	a_sec();
	a_lda(0x3F, IMM);
	a_adc(0x40, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x80, "adc [5a]");
	assert_reg_equals(&sr, 0xF0, "adc [5b]");

	reset_pc();
	reset_status();
	a_clc();
	a_lda(0xC0, IMM);
	a_sbc(0x40, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x7F, "sbc [4a]");
	assert_reg_equals(&sr, 0x71, "sbc [4b]");
	// end: clc, sec, adc, sbc


	// start: decimal mode
	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x05, IMM);
	a_adc(0x05, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x10, "decimal mode adc [1a]");
	assert_reg_equals(&sr, 0x38, "decimal mode adc [1b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x09, IMM);
	a_adc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x10, "decimal mode adc [2a]");
	assert_reg_equals(&sr, 0x38, "decimal mode adc [2b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x50, IMM);
	a_adc(0x49, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x99, "decimal mode adc [3a]");
	assert_reg_equals(&sr, 0xF8, "decimal mode adc [3b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x51, IMM);
	a_adc(0x49, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x00, "decimal mode adc [4a]");
	assert_reg_equals(&sr, 0xF9, "decimal mode adc [4b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x1A, IMM);
	a_adc(0x2B, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x4B, "decimal mode adc [5a]");
	assert_reg_equals(&sr, 0x38, "decimal mode adc [5b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x1A, IMM);
	a_adc(0x7B, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x9B, "decimal mode adc [6a]");
	assert_reg_equals(&sr, 0xF8, "decimal mode adc [6b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x1A, IMM);
	a_adc(0xFB, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x7B, "decimal mode adc [7a]");
	assert_reg_equals(&sr, 0x39, "decimal mode adc [7b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x02, IMM);
	a_adc(0xFB, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x63, "decimal mode adc [8a]");
	assert_reg_equals(&sr, 0x39, "decimal mode adc [8b]");

	reset_pc();
	reset_status();
	a_sed();
	a_lda(0x7A, IMM);
	a_adc(0x01, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x81, "decimal mode adc [9a]");
	assert_reg_equals(&sr, 0xF8, "decimal mode adc [9b]");

	/*reset_pc();
	reset_status();
	a_sed();
	a_lda(0xFF, IMM);
	a_adc(0xFF, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x54, "decimal mode adc [10a]");
	assert_reg_equals(&sr, 0xB9, "decimal mode adc [10b]");*/

	reset_pc();
	reset_status();
	a_sed();
	a_sec();
	a_lda(0x46, IMM);
	a_sbc(0x12, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x34, "decimal mode sbc [1a]");
	assert_reg_equals(&sr, 0x39, "decimal mode sbc [1b]");

	reset_pc();
	reset_status();
	a_sed();
	a_sec();
	a_lda(0x40, IMM);
	a_sbc(0x13, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x27, "decimal mode sbc [2a]");
	assert_reg_equals(&sr, 0x39, "decimal mode sbc [2b]");
	// end: decimal mode


	// start: and, asl, bit
	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	a_and(0x00, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x00, "and [1a]");
	assert_reg_equals(&sr, 0x32, "and [1b]");

	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	a_and(0x12, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x12, "and [2a]");
	assert_reg_equals(&sr, 0x30, "and [2b]");

	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	a_and(0x80, IMM);
	run_6502();
	assert_reg_equals(&ac, 0x80, "and [3a]");
	assert_reg_equals(&sr, 0xB0, "and [3b]");

	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	a_asl(0, ACC);
	run_6502();
	assert_reg_equals(&ac, 0xFE, "asl [1a]");
	assert_reg_equals(&sr, 0xB1, "asl [1b]");

	reset_pc();
	reset_status();
	ram[0x0001] = 0xFE;
	a_asl(0x01, ZP_);
	run_6502();
	assert_reg_equals(&ram[0x0001], 0xFC, "asl [2a]");
	assert_reg_equals(&sr, 0xB1, "asl [2b]");

	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	ram[0x0001] = 0;
	a_bit(0x01, ZP_);
	run_6502();
	assert_reg_equals(&sr, 0x32, "bit [1]");

	reset_pc();
	reset_status();
	a_lda(0xFF, IMM);
	ram[0x0001] = 0xF0;
	a_bit(0x01, ZP_);
	run_6502();
	assert_reg_equals(&sr, 0xF0, "bit [1]");
	// end: and, asl, bit

    return 0;
}