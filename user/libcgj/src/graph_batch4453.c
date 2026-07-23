/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4453: clear bit i in a dense uint8_t byte bitmap.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_clear_byte_u(uint8_t *bits, size_t i);
 *     - Clear bit i in the dense byte array bits[]. Bit 0 is LSB of
 *       bits[0]; bit i lives in bits[i/8] at (i % 8). NULL bits →
 *       no-op and return 0; success returns 1. Idempotent on already-clear.
 *   int __gj_bitmap_clear_byte_u  (alias)
 *   __libcgj_batch4453_marker = "libcgj-batch4453"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Distinct from gj_bitmap_clr_u (batch2853, uint64_t words + nwords),
 * gj_bitmap_clear_bit_u (batch2442), and gj_bitset_clear (batch118).
 * Unique gj_bitmap_clear_byte_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4453_marker[] = "libcgj-batch4453";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clear bit uBit in pBits (caller ensures non-NULL). */
static void
b4453_clear(uint8_t *pBits, size_t uBit)
{
	size_t iByte;
	unsigned uShift;
	uint8_t u8Mask;

	iByte = uBit / 8u;
	uShift = (unsigned)(uBit % 8u);
	u8Mask = (uint8_t)(1u << uShift);
	pBits[iByte] = (uint8_t)(pBits[iByte] & (uint8_t)(~u8Mask));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_clear_byte_u - clear one bit in a dense uint8_t bitmap.
 *
 * bits: mutable dense byte array; may be NULL → no-op, return 0
 * i:    absolute bit index (LSB-first within each byte)
 *
 * Returns 1 on success (bit cleared), 0 when bits is NULL. No libc.
 */
int
gj_bitmap_clear_byte_u(uint8_t *pBits, size_t uI)
{
	if (pBits == NULL) {
		return 0;
	}
	b4453_clear(pBits, uI);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_clear_byte_u(uint8_t *pBits, size_t uI)
    __attribute__((alias("gj_bitmap_clear_byte_u")));
