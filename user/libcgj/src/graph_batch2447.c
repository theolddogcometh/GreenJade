/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2447: first clear bit in a uint64_t word bitmap.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_find_first_zero_u(const uint64_t *words, size_t nwords);
 *     — Return the bit index of the first clear (0) bit in words[0..nwords).
 *       Bit 0 is LSB of words[0]. If every bit is set, return nwords*64.
 *       NULL words or nwords == 0 → 0.
 *   size_t __gj_bitmap_find_first_zero_u  (alias)
 *   __libcgj_batch2447_marker = "libcgj-batch2447"
 *
 * Bitmap exclusive wave (2441-2450). Distinct from gj_bitmap_first_zero
 * (batch535) and gj_bitmap_find_next_zero (batch1057). Unique
 * gj_bitmap_find_first_zero_u surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. Manual CTZ (no builtins required). No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2447_marker[] = "libcgj-batch2447";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of the least-significant set bit in u64W (must be non-zero).
 * Pure loop — no __builtin_ctzll.
 */
static size_t
b2447_ctz64(uint64_t u64W)
{
	size_t uB;

	for (uB = 0u; uB < 64u; uB++) {
		if ((u64W & (UINT64_C(1) << uB)) != 0u) {
			return uB;
		}
	}
	return 0u; /* unreachable when w != 0 */
}

/* Scan for first clear bit; return cWords*64 if none. */
static size_t
b2447_first_zero(const uint64_t *pWords, size_t cWords)
{
	size_t iWord;
	uint64_t u64W;
	uint64_t u64Inv;

	for (iWord = 0u; iWord < cWords; iWord++) {
		u64W = pWords[iWord];
		/* All-ones limb has no clear bit; skip. */
		if (u64W == UINT64_C(0xFFFFFFFFFFFFFFFF)) {
			continue;
		}
		u64Inv = ~u64W;
		return (iWord * 64u) + b2447_ctz64(u64Inv);
	}
	return cWords * 64u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_find_first_zero_u — first clear bit index in a word bitmap.
 *
 * words:  array of nwords 64-bit limbs; bit i lives in words[i/64]
 *         at shift (i % 64). May be NULL only when treated as empty.
 * nwords: number of limbs; 0 is empty.
 *
 * Returns:
 *   0              if words == NULL or nwords == 0
 *   bit index      of the first 0-bit in [0, nwords*64)
 *   nwords * 64    if every bit in the range is 1
 *
 * Does not call libc or builtins. Does not write through words.
 */
size_t
gj_bitmap_find_first_zero_u(const uint64_t *pWords, size_t cWords)
{
	if (pWords == NULL || cWords == 0u) {
		return 0u;
	}
	return b2447_first_zero(pWords, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_find_first_zero_u(const uint64_t *pWords, size_t cWords)
    __attribute__((alias("gj_bitmap_find_first_zero_u")));
