/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch209: dense bitset next-set-bit scan.
 *
 * Extends the batch118 bitset surface with a cursor-style next query.
 * Does not redefine set/clear/test/count/find_first (graph_batch118).
 *
 * Surface (unique symbols):
 *   ssize_t gj_bitset_next(const uint64_t *bits, size_t nbits, size_t start);
 *     — index of the next set bit at or after start, or -1 if none /
 *       empty / NULL / start >= nbits.
 *   __gj_bitset_next  (alias)
 *   __libcgj_batch209_marker = "libcgj-batch209"
 *
 * Layout (same as batch118): bits[i] is word floor(i/64); bit (i%64) is
 * LSB-first within the word. Only indices in [0, nbits) are visible; a
 * partial final word is masked so bits past nbits are ignored.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No third-party bitset source was copied.
 * No errno (freestanding-safe); callers use the ssize_t return only.
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch209_marker[] = "libcgj-batch209";

/* ---- freestanding helpers ---------------------------------------------- */

/* Mask for the low `cRem` bits of a word (1..63). cRem == 0 not used. */
static uint64_t
b209_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset_next — next set bit at or after start.
 *
 * bits: word array of the bitset (may be NULL → -1).
 * nbits: visible length in bits; partial last word is masked.
 * start: first index to consider (inclusive). start >= nbits → -1.
 *
 * Returns the lowest set index in [start, nbits), or -1 when none.
 * Equivalent to find_first when start == 0 (batch118).
 */
ssize_t
gj_bitset_next(const uint64_t *bits, size_t nbits, size_t start)
{
	size_t iWord;
	size_t iBit;
	size_t cFull;
	size_t cRem;
	size_t i;
	uint64_t uWord;

	if (bits == NULL || nbits == 0u || start >= nbits) {
		return (ssize_t)-1;
	}

	cFull = nbits >> 6;
	cRem = nbits & 63u;

	iWord = start >> 6;
	iBit = start & 63u;

	/* First (possibly partial) word: clear bits below start. */
	uWord = bits[iWord];
	if (iBit != 0u) {
		uWord &= ~(((uint64_t)1 << iBit) - (uint64_t)1);
	}

	/* If start is in the trailing partial word, mask past nbits. */
	if (iWord == cFull) {
		/* cRem must be non-zero because start < nbits. */
		uWord &= b209_lo_mask(cRem);
		if (uWord == 0u) {
			return (ssize_t)-1;
		}
		return (ssize_t)((iWord << 6) +
		                 (size_t)__builtin_ctzll(
		                     (unsigned long long)uWord));
	}

	if (uWord != 0u) {
		return (ssize_t)((iWord << 6) +
		                 (size_t)__builtin_ctzll(
		                     (unsigned long long)uWord));
	}

	/* Remaining full words after the start word. */
	for (i = iWord + 1u; i < cFull; i++) {
		uWord = bits[i];
		if (uWord != 0u) {
			return (ssize_t)((i << 6) +
			                 (size_t)__builtin_ctzll(
			                     (unsigned long long)uWord));
		}
	}

	/* Trailing partial word. */
	if (cRem != 0u) {
		uWord = bits[cFull] & b209_lo_mask(cRem);
		if (uWord != 0u) {
			return (ssize_t)((cFull << 6) +
			                 (size_t)__builtin_ctzll(
			                     (unsigned long long)uWord));
		}
	}

	return (ssize_t)-1;
}

ssize_t __gj_bitset_next(const uint64_t *bits, size_t nbits, size_t start)
    __attribute__((alias("gj_bitset_next")));
