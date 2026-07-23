/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1056: find next set bit at or after start.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_find_next_one(const uint64_t *words, size_t nbits,
 *                                  size_t start);
 *     — Index of the lowest set bit in [start, nbits), or nbits if none /
 *       NULL / empty / start >= nbits.
 *   size_t __gj_bitmap_find_next_one  (alias)
 *   __libcgj_batch1056_marker = "libcgj-batch1056"
 *
 * Distinct from gj_bitset_next / gj_bitmap_first_one — unique name.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Manual CTZ (no builtins required).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1056_marker[] = "libcgj-batch1056";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1056_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static size_t
b1056_ctz64(uint64_t u)
{
	size_t b;

	for (b = 0u; b < 64u; b++) {
		if ((u & ((uint64_t)1 << b)) != 0u) {
			return b;
		}
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_find_next_one — next set bit index at or after start.
 */
size_t
gj_bitmap_find_next_one(const uint64_t *words, size_t nbits, size_t start)
{
	size_t cFull;
	size_t cRem;
	size_t iWord;
	size_t iBit;
	size_t i;
	uint64_t uWord;

	if (words == NULL || nbits == 0u || start >= nbits) {
		return nbits;
	}

	cFull = nbits >> 6;
	cRem = nbits & 63u;
	iWord = start >> 6;
	iBit = start & 63u;

	uWord = words[iWord];
	if (iBit != 0u) {
		uWord &= ~(((uint64_t)1 << iBit) - (uint64_t)1);
	}

	if (iWord == cFull) {
		uWord &= b1056_lo_mask(cRem);
		if (uWord == 0u) {
			return nbits;
		}
		return (iWord << 6) + b1056_ctz64(uWord);
	}

	if (uWord != 0u) {
		return (iWord << 6) + b1056_ctz64(uWord);
	}

	for (i = iWord + 1u; i < cFull; i++) {
		uWord = words[i];
		if (uWord != 0u) {
			return (i << 6) + b1056_ctz64(uWord);
		}
	}

	if (cRem != 0u) {
		uWord = words[cFull] & b1056_lo_mask(cRem);
		if (uWord != 0u) {
			return (cFull << 6) + b1056_ctz64(uWord);
		}
	}
	return nbits;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_find_next_one(const uint64_t *words, size_t nbits,
    size_t start) __attribute__((alias("gj_bitmap_find_next_one")));
