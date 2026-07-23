/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2441: set one bit in a uint64_t word bitmap.
 *
 * Surface (unique symbols):
 *   void gj_bitmap_set_bit_u(uint64_t *words, size_t nwords, size_t bit);
 *     — Set bit `bit` in the little-endian word array words[0..nwords).
 *       Bit 0 is LSB of words[0]. NULL words, nwords == 0, or bit out of
 *       range (bit >= nwords*64) → no-op. Setting an already-set bit is
 *       still success (idempotent).
 *   void __gj_bitmap_set_bit_u  (alias)
 *   __libcgj_batch2441_marker = "libcgj-batch2441"
 *
 * Bitmap exclusive wave (2441-2450). Distinct from gj_bitset_set
 * (batch118, nbits), gj_bset_set (batch491, unbounded), and
 * gj_bitmap_set_range (batch1051). Unique gj_bitmap_set_bit_u only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2441_marker[] = "libcgj-batch2441";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uBit is inside [0, cWords*64). Guards size_t overflow. */
static int
b2441_in_range(size_t cWords, size_t uBit)
{
	size_t uLimit;

	if (cWords == 0u) {
		return 0;
	}
	if (cWords > (SIZE_MAX / 64u)) {
		uLimit = SIZE_MAX;
	} else {
		uLimit = cWords * 64u;
	}
	if (uBit >= uLimit) {
		return 0;
	}
	return 1;
}

/* Set one bit at absolute index uBit (caller validated). */
static void
b2441_set(uint64_t *pWords, size_t uBit)
{
	size_t iWord;
	size_t uShift;

	iWord = uBit / 64u;
	uShift = uBit % 64u;
	pWords[iWord] |= (UINT64_C(1) << uShift);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_set_bit_u — set one bit in a dense word bitmap.
 *
 * words:  mutable array of nwords 64-bit limbs
 * nwords: number of limbs
 * bit:    absolute bit index to set (LSB-first within each word)
 *
 * No-op on NULL / empty / out of range. Does not call libc.
 */
void
gj_bitmap_set_bit_u(uint64_t *pWords, size_t cWords, size_t uBit)
{
	if (pWords == NULL) {
		return;
	}
	if (!b2441_in_range(cWords, uBit)) {
		return;
	}
	b2441_set(pWords, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_bitmap_set_bit_u(uint64_t *pWords, size_t cWords, size_t uBit)
    __attribute__((alias("gj_bitmap_set_bit_u")));
