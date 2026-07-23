/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1053: test whether any bit is set in a range.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_test_range_any(const uint64_t *words, size_t nbits,
 *                                size_t lo, size_t hi);
 *     — Return 1 if any bit in [lo, hi) (clamped to nbits) is set;
 *       else 0. NULL words or empty range → 0.
 *   int __gj_bitmap_test_range_any  (alias)
 *   __libcgj_batch1053_marker = "libcgj-batch1053"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1053_marker[] = "libcgj-batch1053";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1053_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static uint64_t
b1053_hi_from(size_t iBit)
{
	if (iBit == 0u) {
		return ~(uint64_t)0;
	}
	return ~(((uint64_t)1 << iBit) - (uint64_t)1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_test_range_any — 1 if any set bit in [lo, hi).
 */
int
gj_bitmap_test_range_any(const uint64_t *words, size_t nbits, size_t lo,
    size_t hi)
{
	size_t iLoWord;
	size_t iHiWord;
	size_t iLoBit;
	size_t iHiBit;
	size_t i;
	uint64_t uWord;

	if (words == NULL) {
		return 0;
	}
	if (hi > nbits) {
		hi = nbits;
	}
	if (lo >= hi) {
		return 0;
	}

	iLoWord = lo >> 6;
	iHiWord = (hi - 1u) >> 6;
	iLoBit = lo & 63u;
	iHiBit = hi & 63u;

	if (iLoWord == iHiWord) {
		uWord = words[iLoWord] & b1053_hi_from(iLoBit);
		if (iHiBit != 0u) {
			uWord &= b1053_lo_mask(iHiBit);
		}
		return (uWord != 0u) ? 1 : 0;
	}

	uWord = words[iLoWord] & b1053_hi_from(iLoBit);
	if (uWord != 0u) {
		return 1;
	}
	for (i = iLoWord + 1u; i < iHiWord; i++) {
		if (words[i] != 0u) {
			return 1;
		}
	}
	uWord = words[iHiWord];
	if (iHiBit != 0u) {
		uWord &= b1053_lo_mask(iHiBit);
	}
	return (uWord != 0u) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_test_range_any(const uint64_t *words, size_t nbits, size_t lo,
    size_t hi) __attribute__((alias("gj_bitmap_test_range_any")));
