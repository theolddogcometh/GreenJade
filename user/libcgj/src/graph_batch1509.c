/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1509: validate Huffman code-length array bounds.
 *
 * Surface (unique symbols):
 *   int gj_huff_canonical_len_ok(const uint8_t *lens, size_t nsyms,
 *                                unsigned max_bits);
 *     — Validate a per-symbol code length array for building a canonical
 *       Huffman code:
 *         * lens non-NULL when nsyms > 0
 *         * max_bits in 1..15 (table-friendly bound)
 *         * nsyms == 0 is valid (empty alphabet)
 *         * each lens[i] is in 0..max_bits (0 = unused symbol)
 *         * Kraft inequality: the set of non-zero lengths is not
 *           oversubscribed (left never goes negative while scanning
 *           bit lengths 1..max_bits). Incomplete trees (left > 0) are OK.
 *       Returns 1 if valid, 0 if invalid / bad args.
 *   int __gj_huff_canonical_len_ok  (alias)
 *   __libcgj_batch1509_marker = "libcgj-batch1509"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1509_marker[] = "libcgj-batch1509";

/* Canonical Huffman codes practically use at most 15 bits (DEFLATE). */
#define B1509_MAX_BITS_LIM 15u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Count symbols per bit length into aCount[0..max_bits].
 * Returns 0 on success, -1 if any length is out of range.
 */
static int
b1509_hist(const uint8_t *pLens, size_t cSyms, unsigned nMaxBits,
    size_t *aCount)
{
	size_t i;
	unsigned nB;

	for (nB = 0u; nB <= nMaxBits; nB++) {
		aCount[nB] = 0u;
	}
	for (i = 0u; i < cSyms; i++) {
		nB = (unsigned)pLens[i];
		if (nB > nMaxBits) {
			return -1;
		}
		aCount[nB]++;
	}
	return 0;
}

/*
 * Kraft oversubscription check (DEFLATE-style):
 *   left = 1
 *   for bits = 1..max:
 *     left = (left << 1) - count[bits]
 *     if left < 0: oversubscribed
 * Incomplete (left > 0) is accepted.
 * Counts are size_t; kraft left uses int64 to avoid mid-loop overflow.
 */
static int
b1509_kraft_ok(const size_t *aCount, unsigned nMaxBits)
{
	int64_t i64Left;
	unsigned nB;

	i64Left = 1;
	for (nB = 1u; nB <= nMaxBits; nB++) {
		i64Left <<= 1;
		i64Left -= (int64_t)aCount[nB];
		if (i64Left < 0) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_huff_canonical_len_ok — bounds + Kraft check on code lengths.
 */
int
gj_huff_canonical_len_ok(const uint8_t *pLens, size_t cSyms,
    unsigned nMaxBits)
{
	size_t aCount[B1509_MAX_BITS_LIM + 1u];

	if (nMaxBits == 0u || nMaxBits > B1509_MAX_BITS_LIM) {
		return 0;
	}
	if (cSyms == 0u) {
		return 1;
	}
	if (pLens == NULL) {
		return 0;
	}

	if (b1509_hist(pLens, cSyms, nMaxBits, aCount) != 0) {
		return 0;
	}

	/*
	 * All-zero lengths: no codes assigned. Treat as valid empty code
	 * (encoder may reserve the alphabet without emitting symbols).
	 */
	if (aCount[0] == cSyms) {
		return 1;
	}

	return b1509_kraft_ok(aCount, nMaxBits);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_huff_canonical_len_ok(const uint8_t *pLens, size_t cSyms,
    unsigned nMaxBits)
    __attribute__((alias("gj_huff_canonical_len_ok")));
