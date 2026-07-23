/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch118: dense bitset over uint64_t words.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Userspace may be built with -msse2; this TU does not require SSE.
 * No third-party bitset source was copied.
 *
 * Surface:
 *   void gj_bitset_set(uint64_t *bits, size_t nbits, size_t i);
 *   void gj_bitset_clear(uint64_t *bits, size_t nbits, size_t i);
 *   int gj_bitset_test(const uint64_t *bits, size_t nbits, size_t i);
 *   size_t gj_bitset_count(const uint64_t *bits, size_t nbits);
 *   ssize_t gj_bitset_find_first(const uint64_t *bits, size_t nbits);
 *   __libcgj_batch118_marker = "libcgj-batch118"
 *
 * Layout: bits[i] is word floor(i/64); bit (i%64) is LSB-first within the
 * word. Only indices in [0, nbits) are visible; a partial final word is
 * masked so bits past nbits are ignored by count / find_first.
 * set/clear/test on i >= nbits or NULL bits are no-ops (test → 0).
 * find_first returns the lowest set index, or -1 when none / empty.
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch118_marker[] = "libcgj-batch118";

/* ---- freestanding helpers ---------------------------------------------- */

/* Mask for the low `cRem` bits of a word (1..63). cRem == 0 not used. */
static uint64_t
b118_lo_mask(size_t cRem)
{
	return ((uint64_t)1 << cRem) - (uint64_t)1;
}

/* ---- public API -------------------------------------------------------- */

void
gj_bitset_set(uint64_t *pBits, size_t cBits, size_t iBit)
{
	if (pBits == NULL || iBit >= cBits) {
		return;
	}
	pBits[iBit >> 6] |= (uint64_t)1 << (iBit & 63u);
}

void
gj_bitset_clear(uint64_t *pBits, size_t cBits, size_t iBit)
{
	if (pBits == NULL || iBit >= cBits) {
		return;
	}
	pBits[iBit >> 6] &= ~((uint64_t)1 << (iBit & 63u));
}

int
gj_bitset_test(const uint64_t *pBits, size_t cBits, size_t iBit)
{
	if (pBits == NULL || iBit >= cBits) {
		return 0;
	}
	return (int)((pBits[iBit >> 6] >> (iBit & 63u)) & (uint64_t)1);
}

size_t
gj_bitset_count(const uint64_t *pBits, size_t cBits)
{
	size_t cFull;
	size_t cRem;
	size_t cPop;
	size_t i;

	if (pBits == NULL || cBits == 0u) {
		return 0u;
	}

	cFull = cBits >> 6;
	cRem = cBits & 63u;
	cPop = 0u;

	for (i = 0u; i < cFull; i++) {
		cPop += (size_t)__builtin_popcountll((unsigned long long)pBits[i]);
	}
	if (cRem != 0u) {
		cPop += (size_t)__builtin_popcountll(
		    (unsigned long long)(pBits[cFull] & b118_lo_mask(cRem)));
	}
	return cPop;
}

ssize_t
gj_bitset_find_first(const uint64_t *pBits, size_t cBits)
{
	size_t cFull;
	size_t cRem;
	size_t i;
	uint64_t uWord;

	if (pBits == NULL || cBits == 0u) {
		return (ssize_t)-1;
	}

	cFull = cBits >> 6;
	cRem = cBits & 63u;

	for (i = 0u; i < cFull; i++) {
		uWord = pBits[i];
		if (uWord != 0u) {
			return (ssize_t)((i << 6) +
			                 (size_t)__builtin_ctzll(
			                     (unsigned long long)uWord));
		}
	}
	if (cRem != 0u) {
		uWord = pBits[cFull] & b118_lo_mask(cRem);
		if (uWord != 0u) {
			return (ssize_t)((cFull << 6) +
			                 (size_t)__builtin_ctzll(
			                     (unsigned long long)uWord));
		}
	}
	return (ssize_t)-1;
}
