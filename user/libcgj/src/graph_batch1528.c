/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1528: bitmap free (clear) one allocated bit.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_free_bit(uint64_t *words, size_t nwords, size_t bit);
 *     — Clear bit `bit` in the little-endian word array words[0..nwords).
 *       Bit 0 is LSB of words[0]. Returns 1 on success, 0 if words is
 *       NULL, nwords == 0, or bit is out of range (bit >= nwords*64).
 *       Clearing an already-clear bit is still success (idempotent).
 *   int __gj_bitmap_free_bit  (alias)
 *   __libcgj_batch1528_marker = "libcgj-batch1528"
 *
 * Pair with gj_bitmap_alloc_first_zero (batch1527). Unique surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1528_marker[] = "libcgj-batch1528";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_free_bit — mark bit free (clear) in a word bitmap.
 *
 * pWords: mutable array of cWords 64-bit limbs
 * cWords: number of limbs
 * uBit:   absolute bit index to clear
 *
 * Returns 1 on success, 0 on bad args / out of range.
 */
int
gj_bitmap_free_bit(uint64_t *pWords, size_t cWords, size_t uBit)
{
	size_t iWord;
	size_t uShift;
	size_t uLimit;

	if (pWords == NULL || cWords == 0u) {
		return 0;
	}

	/* nwords * 64 may overflow size_t for huge nwords; guard. */
	if (cWords > (SIZE_MAX / 64u)) {
		uLimit = SIZE_MAX;
	} else {
		uLimit = cWords * 64u;
	}
	if (uBit >= uLimit) {
		return 0;
	}

	iWord = uBit / 64u;
	uShift = uBit % 64u;
	pWords[iWord] &= ~(UINT64_C(1) << uShift);
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_free_bit(uint64_t *pWords, size_t cWords, size_t uBit)
    __attribute__((alias("gj_bitmap_free_bit")));
