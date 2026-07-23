/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3167: test a single bit in a uint16_t bitset.
 *
 * Surface (unique symbols):
 *   int gj_bitset16_test_u(uint16_t bits, unsigned b);
 *     - Return 1 if bit b of bits is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 16 yields 0 (no shift UB).
 *   int __gj_bitset16_test_u  (alias)
 *   __libcgj_batch3167_marker = "libcgj-batch3167"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bitset8_test_u (batch3164) / gj_bit_test_u32 (batch2721) — unique
 * gj_bitset16_test_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3167_marker[] = "libcgj-batch3167";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3167_test(uint16_t u16Bits, unsigned uB)
{
	if (uB >= 16u) {
		return 0;
	}
	return ((u16Bits >> uB) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset16_test_u - test whether bit b of a 16-bit bitset is set.
 *
 * bits: 16-bit word used as a dense bitset
 * b:    bit index (0 = least significant bit)
 *
 * Returns 1 if (bits & (1 << b)) is non-zero and b < 16; returns 0 if
 * b >= 16 or the selected bit is clear. No parent wires.
 */
int
gj_bitset16_test_u(uint16_t u16Bits, unsigned uB)
{
	(void)NULL;
	return b3167_test(u16Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset16_test_u(uint16_t u16Bits, unsigned uB)
    __attribute__((alias("gj_bitset16_test_u")));
