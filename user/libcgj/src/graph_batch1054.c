/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1054: test whether all bits are set in a range.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_test_range_all(const uint64_t *words, size_t nbits,
 *                                size_t lo, size_t hi);
 *     — Return 1 if every bit in [lo, hi) (clamped to nbits) is set;
 *       else 0. NULL words or empty range → 0 (vacuous empty → 0 for
 *       conservative callers; non-empty full range required).
 *   int __gj_bitmap_test_range_all  (alias)
 *   __libcgj_batch1054_marker = "libcgj-batch1054"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1054_marker[] = "libcgj-batch1054";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1054_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static uint64_t
b1054_hi_from(size_t iBit)
{
	if (iBit == 0u) {
		return ~(uint64_t)0;
	}
	return ~(((uint64_t)1 << iBit) - (uint64_t)1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_test_range_all — 1 if all bits in [lo, hi) are set.
 *
 * Empty range (lo >= hi after clamp) returns 0.
 */
int
gj_bitmap_test_range_all(const uint64_t *words, size_t nbits, size_t lo,
    size_t hi)
{
	size_t iLoWord;
	size_t iHiWord;
	size_t iLoBit;
	size_t iHiBit;
	size_t i;
	uint64_t uMask;
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
		uMask = b1054_hi_from(iLoBit);
		if (iHiBit != 0u) {
			uMask &= b1054_lo_mask(iHiBit);
		}
		return ((words[iLoWord] & uMask) == uMask) ? 1 : 0;
	}

	uMask = b1054_hi_from(iLoBit);
	if ((words[iLoWord] & uMask) != uMask) {
		return 0;
	}
	for (i = iLoWord + 1u; i < iHiWord; i++) {
		if (words[i] != ~(uint64_t)0) {
			return 0;
		}
	}
	if (iHiBit == 0u) {
		return (words[iHiWord] == ~(uint64_t)0) ? 1 : 0;
	}
	uMask = b1054_lo_mask(iHiBit);
	uWord = words[iHiWord] & uMask;
	return (uWord == uMask) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_test_range_all(const uint64_t *words, size_t nbits, size_t lo,
    size_t hi) __attribute__((alias("gj_bitmap_test_range_all")));
