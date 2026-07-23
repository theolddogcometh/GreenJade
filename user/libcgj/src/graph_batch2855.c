/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2855: count set bits over full uint64_t words.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_count_u(const uint64_t *words, size_t nwords);
 *     — Sum of set bits across nwords full uint64_t limbs (all 64 bits of
 *       each word count; no partial-word mask). NULL words or nwords == 0
 *       → 0.
 *   size_t __gj_bitmap_count_u  (alias)
 *   __libcgj_batch2855_marker = "libcgj-batch2855"
 *
 * Bitmap exclusive wave (2851-2860). Distinct from gj_bitmap_count_set_u
 * (batch2445), gj_bitset_count (batch118), gj_bitset_pop_all (batch492),
 * and gj_bitmap_popcount_range (batch1055). Unique gj_bitmap_count_u.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. Software popcount (no builtins required). No
 * third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2855_marker[] = "libcgj-batch2855";

/* ---- freestanding helpers ---------------------------------------------- */

/* Software popcount of one uint64 limb (Kernighan clear-lowest-set). */
static size_t
b2855_pop64(uint64_t u64W)
{
	size_t cPop;

	cPop = 0u;
	while (u64W != 0u) {
		cPop++;
		u64W &= u64W - 1u;
	}
	return cPop;
}

/* Sum popcounts over cWords limbs. */
static size_t
b2855_count(const uint64_t *pWords, size_t cWords)
{
	size_t cPop;
	size_t iWord;

	cPop = 0u;
	for (iWord = 0u; iWord < cWords; iWord++) {
		cPop += b2855_pop64(pWords[iWord]);
	}
	return cPop;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_count_u — count set bits over nwords full words.
 *
 * words:  dense bitset word array (may be NULL → 0)
 * nwords: number of uint64_t limbs; every bit in each limb is counted
 *
 * Returns total set-bit count, or 0 when words is NULL / nwords is 0.
 * Does not call libc.
 */
size_t
gj_bitmap_count_u(const uint64_t *pWords, size_t cWords)
{
	if (pWords == NULL || cWords == 0u) {
		return 0u;
	}
	return b2855_count(pWords, cWords);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_count_u(const uint64_t *pWords, size_t cWords)
    __attribute__((alias("gj_bitmap_count_u")));
