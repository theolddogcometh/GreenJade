/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4459: first set bit over nbytes of uint8_t.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_first_set_byte_u(const uint8_t *bits, size_t nbytes);
 *     - Return the absolute index of the lowest set bit across nbytes
 *       full bytes (LSB-first within each byte). Returns nbytes*8 when
 *       none are set. NULL bits or nbytes == 0 → 0.
 *   size_t __gj_bitmap_first_set_byte_u  (alias)
 *   __libcgj_batch4459_marker = "libcgj-batch4459"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Distinct from gj_bitmap_first_set_u (batch2856, uint64_t words),
 * gj_bitmap_find_first_set_u (batch2446), and gj_bitmap_first_one
 * (batch536). Unique gj_bitmap_first_set_byte_u surface only; no
 * multi-def. No parent wires. No __int128. Manual CTZ (no builtins).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4459_marker[] = "libcgj-batch4459";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of the least-significant set bit in u8W (must be non-zero).
 * Pure loop — no __builtin_ctz.
 */
static size_t
b4459_ctz8(uint8_t u8W)
{
	size_t uB;

	for (uB = 0u; uB < 8u; uB++) {
		if ((u8W & (uint8_t)(1u << uB)) != 0u) {
			return uB;
		}
	}
	return 0u; /* unreachable when w != 0 */
}

/* Scan for first set bit; return cBytes*8 if none. */
static size_t
b4459_first_set(const uint8_t *pBits, size_t cBytes)
{
	size_t iByte;
	uint8_t u8W;

	for (iByte = 0u; iByte < cBytes; iByte++) {
		u8W = pBits[iByte];
		if (u8W != 0u) {
			return (iByte * 8u) + b4459_ctz8(u8W);
		}
	}
	return cBytes * 8u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_first_set_byte_u - index of the first set bit in a byte array.
 *
 * bits:   dense byte bitmap (may be NULL → 0)
 * nbytes: number of full bytes; every bit of each byte is visible
 *
 * Layout: bit (iByte*8 + b) is bits[iByte] bit b (LSB = bit 0).
 *
 * Returns:
 *   0              if bits == NULL or nbytes == 0
 *   bit index      of the lowest set bit
 *   nbytes * 8     if no bit is set
 */
size_t
gj_bitmap_first_set_byte_u(const uint8_t *pBits, size_t cBytes)
{
	if (pBits == NULL || cBytes == 0u) {
		return 0u;
	}
	return b4459_first_set(pBits, cBytes);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_first_set_byte_u(const uint8_t *pBits, size_t cBytes)
    __attribute__((alias("gj_bitmap_first_set_byte_u")));
