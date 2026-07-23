/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch725: Hamming(7,4) encode of a 4-bit nibble.
 *
 * Surface (unique symbols):
 *   uint8_t gj_hamming74_encode(uint8_t d4);
 *     — Encode the low 4 data bits of d4 into a (7,4) Hamming codeword
 *       returned in bits [6:0] (bit 7 always 0). Higher bits of d4 are
 *       ignored (masked with 0x0F). Even parity over the classical
 *       position layout (1-indexed positions 1,2,4 are parity; 3,5,6,7
 *       hold data bits d0..d3 with d0 = LSB of the nibble).
 *   uint8_t __gj_hamming74_encode  (alias)
 *   __libcgj_batch725_marker = "libcgj-batch725"
 *
 * Distinct from gj_hamming_distance (batch184), gj_hamming_str
 * (batch213), and gj_hamming_u8 (batch537) — those measure bit
 * differences; this TU only builds a (7,4) codeword. No multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch725_marker[] = "libcgj-batch725";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hamming74_encode — (7,4) Hamming encode of a 4-bit data nibble.
 *
 * d4: data bits in the low nibble (only bits [3:0] are used):
 *       d0 = (d4 >> 0) & 1   → codeword position 3
 *       d1 = (d4 >> 1) & 1   → codeword position 5
 *       d2 = (d4 >> 2) & 1   → codeword position 6
 *       d3 = (d4 >> 3) & 1   → codeword position 7
 *
 * Codeword bit layout (return value, 0-indexed; bit 0 = LSB = pos 1):
 *   bit 0  p1   even parity over positions 1,3,5,7  → d0^d1^d3
 *   bit 1  p2   even parity over positions 2,3,6,7  → d0^d2^d3
 *   bit 2  d0
 *   bit 3  p4   even parity over positions 4,5,6,7  → d1^d2^d3
 *   bit 4  d1
 *   bit 5  d2
 *   bit 6  d3
 *   bit 7  0
 *
 * Always succeeds for any d4 (high nibble ignored). Returns the 7-bit
 * codeword in [0, 0x7F].
 */
uint8_t
gj_hamming74_encode(uint8_t d4)
{
	uint8_t d0;
	uint8_t d1;
	uint8_t d2;
	uint8_t d3;
	uint8_t p1;
	uint8_t p2;
	uint8_t p4;
	uint8_t cw;

	d0 = (uint8_t)(d4 & 1u);
	d1 = (uint8_t)((d4 >> 1) & 1u);
	d2 = (uint8_t)((d4 >> 2) & 1u);
	d3 = (uint8_t)((d4 >> 3) & 1u);

	/* Even parity bits (XOR of covered data bits). */
	p1 = (uint8_t)(d0 ^ d1 ^ d3);
	p2 = (uint8_t)(d0 ^ d2 ^ d3);
	p4 = (uint8_t)(d1 ^ d2 ^ d3);

	cw = (uint8_t)(p1
	    | (uint8_t)(p2 << 1)
	    | (uint8_t)(d0 << 2)
	    | (uint8_t)(p4 << 3)
	    | (uint8_t)(d1 << 4)
	    | (uint8_t)(d2 << 5)
	    | (uint8_t)(d3 << 6));

	return cw;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_hamming74_encode(uint8_t d4)
    __attribute__((alias("gj_hamming74_encode")));
