/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch726: freestanding Hamming(7,4) single-error
 * decode of a 7-bit codeword.
 *
 * Surface (unique symbols):
 *   int gj_hamming74_decode(uint8_t code, uint8_t *data4);
 *     — Decode a Hamming(7,4) codeword in the low 7 bits of code.
 *       Bit layout (LSB = position 1, matching classical 1-based
 *       Hamming numbering):
 *         bit0 = p1, bit1 = p2, bit2 = d1, bit3 = p4,
 *         bit4 = d2, bit5 = d3, bit6 = d4
 *       (bit7 of code is ignored).
 *       Compute the 3-bit syndrome (s1 | s2<<1 | s4<<2). If non-zero,
 *       flip the bit at that 1-based position (single-error correction).
 *       Write the recovered 4 data bits as a low nibble into *data4:
 *         *data4 = d1 | (d2<<1) | (d3<<2) | (d4<<3)  (upper nibble 0).
 *       Returns the syndrome: 0 if no error, or the 1-based error bit
 *       position (1..7) that was corrected. Returns -1 (FAIL) if
 *       data4 is NULL (*data4 untouched on FAIL).
 *   int __gj_hamming74_decode  (alias)
 *   __libcgj_batch726_marker = "libcgj-batch726"
 *
 * Unique gj_ names only; does not redefine buffer/string Hamming
 * distance surfaces (batch184 / batch213 / batch537).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch726_marker[] = "libcgj-batch726";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_hamming74_decode — Hamming(7,4) decode with single-error correction.
 *
 * Parity checks (even parity over positions with the corresponding
 * binary place value set):
 *   s1 = p1 ^ d1 ^ d2 ^ d4   (positions 1,3,5,7)
 *   s2 = p2 ^ d1 ^ d3 ^ d4   (positions 2,3,6,7)
 *   s4 = p4 ^ d2 ^ d3 ^ d4   (positions 4,5,6,7)
 * syndrome = s1 + 2*s2 + 4*s4 ∈ {0..7}; 0 means a valid codeword.
 *
 * Edge cases:
 *   data4 == NULL → -1 (FAIL)
 *   syndrome 0    → no flip; return 0 (OK / clean)
 *   syndrome 1..7 → flip that position, return syndrome
 * High bit of code is never consulted and does not affect the result.
 */
int
gj_hamming74_decode(uint8_t code, uint8_t *data4)
{
	uint8_t uC;
	unsigned s1;
	unsigned s2;
	unsigned s4;
	unsigned uSyn;
	unsigned b0, b1, b2, b3, b4, b5, b6;

	if (data4 == 0) {
		return -1;
	}

	/* Work only on the 7 code bits (positions 1..7 as bits 0..6). */
	uC = (uint8_t)(code & 0x7fu);

	b0 = (unsigned)(uC >> 0) & 1u; /* p1 */
	b1 = (unsigned)(uC >> 1) & 1u; /* p2 */
	b2 = (unsigned)(uC >> 2) & 1u; /* d1 */
	b3 = (unsigned)(uC >> 3) & 1u; /* p4 */
	b4 = (unsigned)(uC >> 4) & 1u; /* d2 */
	b5 = (unsigned)(uC >> 5) & 1u; /* d3 */
	b6 = (unsigned)(uC >> 6) & 1u; /* d4 */

	s1 = b0 ^ b2 ^ b4 ^ b6;
	s2 = b1 ^ b2 ^ b5 ^ b6;
	s4 = b3 ^ b4 ^ b5 ^ b6;
	uSyn = s1 | (s2 << 1) | (s4 << 2);

	/* Correct single-bit error at 1-based position uSyn when nonzero. */
	if (uSyn != 0u) {
		uC = (uint8_t)(uC ^ (uint8_t)(1u << (uSyn - 1u)));
		/* Re-extract data bits after correction. */
		b2 = (unsigned)(uC >> 2) & 1u;
		b4 = (unsigned)(uC >> 4) & 1u;
		b5 = (unsigned)(uC >> 5) & 1u;
		b6 = (unsigned)(uC >> 6) & 1u;
	}

	*data4 = (uint8_t)(b2 | (b4 << 1) | (b5 << 2) | (b6 << 3));
	return (int)uSyn;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_hamming74_decode(uint8_t code, uint8_t *data4)
    __attribute__((alias("gj_hamming74_decode")));
