/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch492: freestanding bitset whole-word popcount.
 *
 * Surface (unique symbols):
 *   size_t gj_bitset_pop_all(const uint64_t *words, size_t nwords);
 *     — sum of set bits across nwords full uint64_t words (all 64 bits of
 *       each word count). NULL words or nwords == 0 → 0.
 *   size_t __gj_bitset_pop_all  (alias)
 *   __libcgj_batch492_marker = "libcgj-batch492"
 *
 * Distinct from:
 *   gj_bitset_count (batch118)     — nbits length, masks partial final word
 *   gj_bitset_popcount (batch245)  — half-open bit range [lo, hi) over nbits
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained (no cross-batch call).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch492_marker[] = "libcgj-batch492";

/* ---- freestanding helpers ---------------------------------------------- */

/* Popcount of one uint64 word. */
static size_t
b492_pop64(uint64_t uWord)
{
	return (size_t)__builtin_popcountll((unsigned long long)uWord);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_pop_all — count set bits over nwords full words.
 *
 * words: dense bitset word array (may be NULL → 0).
 * nwords: number of uint64_t words to scan; every bit in each word is
 *         counted (no partial-word mask).
 */
size_t
gj_bitset_pop_all(const uint64_t *words, size_t nwords)
{
	size_t cPop;
	size_t iWord;

	if (words == NULL || nwords == 0u) {
		return 0u;
	}

	cPop = 0u;
	for (iWord = 0u; iWord < nwords; iWord++) {
		cPop += b492_pop64(words[iWord]);
	}
	return cPop;
}

size_t __gj_bitset_pop_all(const uint64_t *words, size_t nwords)
    __attribute__((alias("gj_bitset_pop_all")));
