/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2446: first set bit in a uint64_t word bitmap.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_find_first_set_u(const uint64_t *words, size_t nwords);
 *     — Return the absolute index of the lowest set bit across nwords
 *       full uint64_t limbs (LSB-first within each word). Returns
 *       nwords*64 when none are set. NULL words or nwords == 0 → 0.
 *   size_t __gj_bitmap_find_first_set_u  (alias)
 *   __libcgj_batch2446_marker = "libcgj-batch2446"
 *
 * Bitmap exclusive wave (2441-2450). Distinct from gj_bitmap_first_one
 * (batch536), gj_bitset_find_first (batch118), and
 * gj_bitmap_find_next_one (batch1056). Unique gj_bitmap_find_first_set_u.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. Manual CTZ (no builtins required). No third-party
 * source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2446_marker[] = "libcgj-batch2446";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of the least-significant set bit in u64W (must be non-zero).
 * Pure loop — no __builtin_ctzll.
 */
static size_t
b2446_ctz64(uint64_t u64W)
{
	size_t uB;

	for (uB = 0u; uB < 64u; uB++) {
		if ((u64W & (UINT64_C(1) << uB)) != 0u) {
			return uB;
		}
	}
	return 0u; /* unreachable when w != 0 */
}

/* Scan for first set bit; return cWords*64 if none. */
static size_t
b2446_first_set(const uint64_t *pWords, size_t cWords)
{
	size_t iWord;
	uint64_t u64W;

	for (iWord = 0u; iWord < cWords; iWord++) {
		u64W = pWords[iWord];
		if (u64W != 0u) {
			return (iWord * 64u) + b2446_ctz64(u64W);
		}
	}
	return cWords * 64u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_find_first_set_u — index of the first set bit in a word array.
 *
 * words:  dense bitset word array (may be NULL → 0)
 * nwords: number of full uint64_t limbs; every bit of each limb is visible
 *
 * Layout: bit (iWord*64 + b) is words[iWord] bit b (LSB = bit 0).
 *
 * Returns:
 *   0              if words == NULL or nwords == 0
 *   bit index      of the lowest set bit
 *   nwords * 64    if no bit is set
 */
size_t
gj_bitmap_find_first_set_u(const uint64_t *pWords, size_t cWords)
{
	if (pWords == NULL || cWords == 0u) {
		return 0u;
	}
	return b2446_first_set(pWords, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_find_first_set_u(const uint64_t *pWords, size_t cWords)
    __attribute__((alias("gj_bitmap_find_first_set_u")));
