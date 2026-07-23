/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4455: count set bits over nbytes of uint8_t.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_count_byte_u(const uint8_t *bits, size_t nbytes);
 *     - Sum of set bits across nbytes full bytes (all 8 bits of each
 *       byte count). NULL bits or nbytes == 0 → 0.
 *   size_t __gj_bitmap_count_byte_u  (alias)
 *   __libcgj_batch4455_marker = "libcgj-batch4455"
 *
 * Exclusive continuum CREATE-ONLY (4451-4460: bitmap byte ops unique).
 * Distinct from gj_bitmap_count_u (batch2855, uint64_t words),
 * gj_bitmap_count_set_u (batch2445), and gj_bitmap_popcount_range
 * (batch1055). Unique gj_bitmap_count_byte_u surface only; no multi-def.
 * No parent wires. No __int128. Software popcount (no builtins).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4455_marker[] = "libcgj-batch4455";

/* ---- freestanding helpers ---------------------------------------------- */

/* Software popcount of one byte (Kernighan clear-lowest-set). */
static size_t
b4455_pop8(uint8_t u8W)
{
	size_t cPop;

	cPop = 0u;
	while (u8W != 0u) {
		cPop++;
		u8W = (uint8_t)(u8W & (uint8_t)(u8W - 1u));
	}
	return cPop;
}

/* Sum popcounts over cBytes bytes. */
static size_t
b4455_count(const uint8_t *pBits, size_t cBytes)
{
	size_t cPop;
	size_t iByte;

	cPop = 0u;
	for (iByte = 0u; iByte < cBytes; iByte++) {
		cPop += b4455_pop8(pBits[iByte]);
	}
	return cPop;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_count_byte_u - count set bits over nbytes full bytes.
 *
 * bits:   dense byte bitmap (may be NULL → 0)
 * nbytes: number of bytes; every bit in each byte is counted
 *
 * Returns total set-bit count, or 0 when bits is NULL / nbytes is 0.
 * Does not call libc.
 */
size_t
gj_bitmap_count_byte_u(const uint8_t *pBits, size_t cBytes)
{
	if (pBits == NULL || cBytes == 0u) {
		return 0u;
	}
	return b4455_count(pBits, cBytes);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_count_byte_u(const uint8_t *pBits, size_t cBytes)
    __attribute__((alias("gj_bitmap_count_byte_u")));
