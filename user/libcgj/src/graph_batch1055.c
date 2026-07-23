/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1055: popcount over a bitmap bit range.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_popcount_range(const uint64_t *words, size_t nbits,
 *                                   size_t lo, size_t hi);
 *     — Count set bits in [lo, hi) with hi clamped to nbits. NULL words
 *       or empty range → 0.
 *   size_t __gj_bitmap_popcount_range  (alias)
 *   __libcgj_batch1055_marker = "libcgj-batch1055"
 *
 * Distinct from gj_bitset_popcount (batch245) — unique gj_bitmap_* name.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1055_marker[] = "libcgj-batch1055";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1055_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static uint64_t
b1055_hi_from(size_t iBit)
{
	if (iBit == 0u) {
		return ~(uint64_t)0;
	}
	return ~(((uint64_t)1 << iBit) - (uint64_t)1);
}

/* Software popcount — works under -fno-builtin without relying on libgcc. */
static size_t
b1055_pop64(uint64_t u)
{
	size_t c = 0u;

	while (u != 0u) {
		c++;
		u &= u - 1u;
	}
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_popcount_range — set-bit count in [lo, hi).
 */
size_t
gj_bitmap_popcount_range(const uint64_t *words, size_t nbits, size_t lo,
    size_t hi)
{
	size_t iLoWord;
	size_t iHiWord;
	size_t iLoBit;
	size_t iHiBit;
	size_t cPop;
	size_t i;
	uint64_t uWord;

	if (words == NULL) {
		return 0u;
	}
	if (hi > nbits) {
		hi = nbits;
	}
	if (lo >= hi) {
		return 0u;
	}

	iLoWord = lo >> 6;
	iHiWord = (hi - 1u) >> 6;
	iLoBit = lo & 63u;
	iHiBit = hi & 63u;

	if (iLoWord == iHiWord) {
		uWord = words[iLoWord] & b1055_hi_from(iLoBit);
		if (iHiBit != 0u) {
			uWord &= b1055_lo_mask(iHiBit);
		}
		return b1055_pop64(uWord);
	}

	cPop = 0u;
	uWord = words[iLoWord] & b1055_hi_from(iLoBit);
	cPop += b1055_pop64(uWord);
	for (i = iLoWord + 1u; i < iHiWord; i++) {
		cPop += b1055_pop64(words[i]);
	}
	uWord = words[iHiWord];
	if (iHiBit != 0u) {
		uWord &= b1055_lo_mask(iHiBit);
	}
	cPop += b1055_pop64(uWord);
	return cPop;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_popcount_range(const uint64_t *words, size_t nbits,
    size_t lo, size_t hi)
    __attribute__((alias("gj_bitmap_popcount_range")));
