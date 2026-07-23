/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3164: test a single bit in a uint8_t bitset.
 *
 * Surface (unique symbols):
 *   int gj_bitset8_test_u(uint8_t bits, unsigned b);
 *     - Return 1 if bit b of bits is set (bit 0 = LSB), else 0.
 *       Out-of-range index b >= 8 yields 0 (no shift UB).
 *   int __gj_bitset8_test_u  (alias)
 *   __libcgj_batch3164_marker = "libcgj-batch3164"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bit_test_u32 (batch2721) / gj_bit_test_u64 (batch653) / dense
 * gj_bitset_test (batch118) — unique gj_bitset8_test_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3164_marker[] = "libcgj-batch3164";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b3164_test(uint8_t u8Bits, unsigned uB)
{
	if (uB >= 8u) {
		return 0;
	}
	return ((u8Bits >> uB) & 1u) != 0u ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset8_test_u - test whether bit b of an 8-bit bitset is set.
 *
 * bits: 8-bit word used as a dense bitset
 * b:    bit index (0 = least significant bit)
 *
 * Returns 1 if (bits & (1 << b)) is non-zero and b < 8; returns 0 if
 * b >= 8 or the selected bit is clear. No parent wires.
 */
int
gj_bitset8_test_u(uint8_t u8Bits, unsigned uB)
{
	(void)NULL;
	return b3164_test(u8Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitset8_test_u(uint8_t u8Bits, unsigned uB)
    __attribute__((alias("gj_bitset8_test_u")));
