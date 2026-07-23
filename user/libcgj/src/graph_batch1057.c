/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1057: find next clear bit at or after start.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_find_next_zero(const uint64_t *words, size_t nbits,
 *                                   size_t start);
 *     — Index of the lowest clear bit in [start, nbits), or nbits if
 *       none / NULL / empty / start >= nbits.
 *   size_t __gj_bitmap_find_next_zero  (alias)
 *   __libcgj_batch1057_marker = "libcgj-batch1057"
 *
 * Distinct from gj_bitmap_first_zero / gj_bitmap_find_next_one.
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Manual CTZ of inverted words.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1057_marker[] = "libcgj-batch1057";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1057_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

static size_t
b1057_ctz64(uint64_t u)
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
 * gj_bitmap_find_next_zero — next clear bit index at or after start.
 */
size_t
gj_bitmap_find_next_zero(const uint64_t *words, size_t nbits, size_t start)
{
	size_t cFull;
	size_t cRem;
	size_t iWord;
	size_t iBit;
	size_t i;
	uint64_t uWord;
	uint64_t uInv;

	if (words == NULL || nbits == 0u || start >= nbits) {
		return nbits;
	}

	cFull = nbits >> 6;
	cRem = nbits & 63u;
	iWord = start >> 6;
	iBit = start & 63u;

	/* Invert: zeros become ones for CTZ. Mask off bits below start. */
	uWord = words[iWord];
	uInv = ~uWord;
	if (iBit != 0u) {
		uInv &= ~(((uint64_t)1 << iBit) - (uint64_t)1);
	}

	if (iWord == cFull) {
		uInv &= b1057_lo_mask(cRem);
		if (uInv == 0u) {
			return nbits;
		}
		return (iWord << 6) + b1057_ctz64(uInv);
	}

	if (uInv != 0u) {
		return (iWord << 6) + b1057_ctz64(uInv);
	}

	for (i = iWord + 1u; i < cFull; i++) {
		uInv = ~words[i];
		if (uInv != 0u) {
			return (i << 6) + b1057_ctz64(uInv);
		}
	}

	if (cRem != 0u) {
		uInv = (~words[cFull]) & b1057_lo_mask(cRem);
		if (uInv != 0u) {
			return (cFull << 6) + b1057_ctz64(uInv);
		}
	}
	return nbits;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_find_next_zero(const uint64_t *words, size_t nbits,
    size_t start) __attribute__((alias("gj_bitmap_find_next_zero")));
