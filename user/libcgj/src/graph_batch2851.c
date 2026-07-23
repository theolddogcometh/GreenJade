/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2851: test one bit in a uint64_t word bitmap.
 *
 * Surface (unique symbols):
 *   int gj_bitmap_test_u(const uint64_t *words, size_t nwords, size_t bit);
 *     — Return 1 if bit `bit` is set in words[0..nwords), else 0.
 *       Bit 0 is LSB of words[0]. NULL words, nwords == 0, or bit out
 *       of range (bit >= nwords*64) → 0.
 *   int __gj_bitmap_test_u  (alias)
 *   __libcgj_batch2851_marker = "libcgj-batch2851"
 *
 * Bitmap exclusive wave (2851-2860). Distinct from gj_bitmap_test_bit_u
 * (batch2443), gj_bitset_test (batch118), and gj_bset_test (batch491).
 * Unique gj_bitmap_test_u surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2851_marker[] = "libcgj-batch2851";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if uBit is inside [0, cWords*64). Guards size_t overflow. */
static int
b2851_in_range(size_t cWords, size_t uBit)
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

/* 1 if bit uBit is set (caller validated). */
static int
b2851_test(const uint64_t *pWords, size_t uBit)
{
	size_t iWord;
	size_t uShift;

	iWord = uBit / 64u;
	uShift = uBit % 64u;
	if ((pWords[iWord] & (UINT64_C(1) << uShift)) != 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_test_u — test one bit in a dense word bitmap.
 *
 * words:  array of nwords 64-bit limbs (read-only)
 * nwords: number of limbs
 * bit:    absolute bit index to test (LSB-first within each word)
 *
 * Returns 1 if set, else 0 (including NULL / empty / OOR). No libc.
 */
int
gj_bitmap_test_u(const uint64_t *pWords, size_t cWords, size_t uBit)
{
	if (pWords == NULL) {
		return 0;
	}
	if (!b2851_in_range(cWords, uBit)) {
		return 0;
	}
	return b2851_test(pWords, uBit);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_bitmap_test_u(const uint64_t *pWords, size_t cWords, size_t uBit)
    __attribute__((alias("gj_bitmap_test_u")));
