/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1052: clear a half-open bit range in a bitmap.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_clear_range(uint64_t *words, size_t nbits,
 *                              size_t lo, size_t hi);
 *     — Clear every bit in [lo, hi) within nbits (hi clamped to nbits).
 *       No-op if words == NULL or lo >= hi after clamp.
 *   void __gj_bitmap_clear_range  (alias)
 *   __libcgj_batch1052_marker = "libcgj-batch1052"
 *
 * Distinct from gj_bitset_clear (single bit) and gj_bitmap_set_range.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1052_marker[] = "libcgj-batch1052";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1052_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static uint64_t
b1052_hi_from(size_t iBit)
{
	if (iBit == 0u) {
		return ~(uint64_t)0;
	}
	return ~(((uint64_t)1 << iBit) - (uint64_t)1);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_clear_range — clear bits in [lo, hi).
 */
void
gj_bitmap_clear_range(uint64_t *words, size_t nbits, size_t lo, size_t hi)
{
	size_t iLoWord;
	size_t iHiWord;
	size_t iLoBit;
	size_t iHiBit;
	size_t i;
	uint64_t uMask;

	if (words == NULL) {
		return;
	}
	if (hi > nbits) {
		hi = nbits;
	}
	if (lo >= hi) {
		return;
	}

	iLoWord = lo >> 6;
	iHiWord = (hi - 1u) >> 6;
	iLoBit = lo & 63u;
	iHiBit = hi & 63u;

	if (iLoWord == iHiWord) {
		uMask = b1052_hi_from(iLoBit);
		if (iHiBit != 0u) {
			uMask &= b1052_lo_mask(iHiBit);
		}
		words[iLoWord] &= ~uMask;
		return;
	}

	words[iLoWord] &= ~b1052_hi_from(iLoBit);
	for (i = iLoWord + 1u; i < iHiWord; i++) {
		words[i] = 0u;
	}
	if (iHiBit == 0u) {
		words[iHiWord] = 0u;
	} else {
		words[iHiWord] &= ~b1052_lo_mask(iHiBit);
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_clear_range(uint64_t *words, size_t nbits, size_t lo,
    size_t hi) __attribute__((alias("gj_bitmap_clear_range")));
