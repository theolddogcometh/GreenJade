/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4451: test bit i in a dense uint8_t byte bitmap.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_test_byte_u(const uint8_t *bits, size_t i);
 *     - Return 1 if bit i is set in the dense byte array bits[], else 0.
 *       Bit 0 is LSB of bits[0]; bit i lives in bits[i/8] at (i % 8).
 *       NULL bits → 0. No upper bound (caller owns storage).
 *   int __gj_bitmap_test_byte_u  (alias)
 *   __libcgj_batch4451_marker = "libcgj-batch4451"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Distinct from gj_bitmap_test_u (batch2851, uint64_t words + nwords),
 * gj_bitmap_test_bit_u (batch2443), and gj_bitset_test (batch118).
 * Unique gj_bitmap_test_byte_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4451_marker[] = "libcgj-batch4451";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bit uBit is set in pBits (caller ensures non-NULL). */
static int
b4451_test(const uint8_t *pBits, size_t uBit)
{
	size_t iByte;
	unsigned uShift;
	uint8_t u8Mask;

	iByte = uBit / 8u;
	uShift = (unsigned)(uBit % 8u);
	u8Mask = (uint8_t)(1u << uShift);
	if ((pBits[iByte] & u8Mask) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_test_byte_u - test one bit in a dense uint8_t bitmap.
 *
 * bits: dense byte array (read-only); may be NULL → 0
 * i:    absolute bit index (LSB-first within each byte)
 *
 * Returns 1 if set, else 0. No libc. No parent wires.
 */
int
gj_bitmap_test_byte_u(const uint8_t *pBits, size_t uI)
{
	if (pBits == NULL) {
		return 0;
	}
	return b4451_test(pBits, uI);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_test_byte_u(const uint8_t *pBits, size_t uI)
    __attribute__((alias("gj_bitmap_test_byte_u")));
