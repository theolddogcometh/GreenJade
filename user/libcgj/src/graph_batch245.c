/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch245: dense bitset range popcount.
 *
 * Extends the batch118/209 bitset surface with a half-open range count.
 * Does not redefine set/clear/test/count/find_first (graph_batch118) or
 * next (graph_batch209).
 *
 * Surface (unique symbols):
 *   size_t gj_bitset_popcount(const uint64_t *words, size_t nbits,
 *                             size_t lo, size_t hi);
 *     — number of set bits in [lo, hi). hi is clamped to nbits.
 *       lo >= hi (after clamp) or NULL words → 0.
 *   __gj_bitset_popcount  (alias)
 *   __libcgj_batch245_marker = "libcgj-batch245"
 *
 * Layout (same as batch118): words[i] is word floor(i/64); bit (i%64) is
 * LSB-first within the word. Only indices in [0, nbits) are visible;
 * clamping hi to nbits keeps a partial final word safe.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party bitset source was copied.
 * No malloc, no errno.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch245_marker[] = "libcgj-batch245";

/* ---- freestanding helpers ---------------------------------------------- */

/* Mask bits [0, cRem) of a word. cRem in 1..63; cRem == 0 not used. */
static uint64_t
b245_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

/* Mask bits [iBit, 64) of a word. iBit in 0..63. */
static uint64_t
b245_hi_from(size_t iBit)
{
	if (iBit == 0u) {
		return ~(uint64_t)0;
	}
	return ~(((uint64_t)1 << iBit) - (uint64_t)1);
}

/* Popcount of one uint64 word. */
static size_t
b245_pop64(uint64_t u)
{
	return (size_t)__builtin_popcountll((unsigned long long)u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_popcount — count set bits in half-open range [lo, hi).
 *
 * words: dense bitset word array (may be NULL → 0).
 * nbits: visible length in bits; hi is clamped to nbits before counting.
 * lo, hi: half-open range. After clamp, lo >= hi → 0.
 *
 * Equivalent to gj_bitset_count (batch118) when lo == 0 and hi >= nbits
 * (after clamp hi == nbits).
 */
size_t
gj_bitset_popcount(const uint64_t *words, size_t nbits, size_t lo, size_t hi)
{
	size_t iLoWord;
	size_t iHiWord;
	size_t iLoBit;
	size_t iHiBit;
	size_t cPop;
	size_t i;
	uint64_t uWord;

	if (words == NULL) {
		return 0u;
	}
	if (hi > nbits) {
		hi = nbits;
	}
	if (lo >= hi) {
		return 0u;
	}

	iLoWord = lo >> 6;
	iHiWord = (hi - 1u) >> 6;
	iLoBit = lo & 63u;
	iHiBit = hi & 63u; /* exclusive end bit within end word; 0 if aligned */

	/* Entire range sits inside a single word. */
	if (iLoWord == iHiWord) {
		uWord = words[iLoWord] & b245_hi_from(iLoBit);
		if (iHiBit != 0u) {
			uWord &= b245_lo_mask(iHiBit);
		}
		/* When hi is word-aligned, iHiBit == 0 and the range runs
		 * through bit 63 of this word; no high-side mask needed. */
		return b245_pop64(uWord);
	}

	cPop = 0u;

	/* Leading partial word: bits [iLoBit, 64). */
	uWord = words[iLoWord] & b245_hi_from(iLoBit);
	cPop += b245_pop64(uWord);

	/* Full middle words. */
	for (i = iLoWord + 1u; i < iHiWord; i++) {
		cPop += b245_pop64(words[i]);
	}

	/* Trailing partial or full end word. */
	uWord = words[iHiWord];
	if (iHiBit != 0u) {
		uWord &= b245_lo_mask(iHiBit);
	}
	/* iHiBit == 0 means hi is word-aligned, so the end word is full
	 * (bits 0..63), which is correct for index (hi-1) in that word. */
	cPop += b245_pop64(uWord);

	return cPop;
}

size_t __gj_bitset_popcount(const uint64_t *words, size_t nbits, size_t lo,
                            size_t hi)
    __attribute__((alias("gj_bitset_popcount")));
