/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch536: freestanding dense bitmap first-set-bit.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_first_one(const uint64_t *words, size_t nwords);
 *     — Return the absolute index of the lowest set bit across nwords
 *       full uint64_t words (LSB-first within each word). Returns
 *       nwords*64 when none are set (or words is NULL / nwords is 0).
 *   size_t __gj_bitmap_first_one  (alias)
 *   __libcgj_batch536_marker = "libcgj-batch536"
 *
 * Distinct from:
 *   gj_bitset_find_first (batch118) — nbits length, masks partial final
 *                                     word, returns ssize_t -1 on empty
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Self-contained (no cross-batch call).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch536_marker[] = "libcgj-batch536";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_first_one — index of the first set bit in a dense word array.
 *
 * words:  dense bitset word array (may be NULL → treat as empty).
 * nwords: number of full uint64_t words; every bit of each word is
 *         visible (no partial-word mask).
 *
 * Layout: bit (iWord*64 + b) is words[iWord] bit b (LSB = bit 0).
 *
 * Returns:
 *   absolute bit index of the lowest set bit, or
 *   nwords * 64 when no bit is set (also when words == NULL or nwords == 0).
 */
size_t
gj_bitmap_first_one(const uint64_t *words, size_t nwords)
{
	size_t iWord;
	uint64_t uWord;

	if (words == NULL || nwords == 0u) {
		return nwords * 64u;
	}

	for (iWord = 0u; iWord < nwords; iWord++) {
		uWord = words[iWord];
		if (uWord != 0u) {
			return (iWord * 64u) +
			       (size_t)__builtin_ctzll(
			           (unsigned long long)uWord);
		}
	}
	return nwords * 64u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_first_one(const uint64_t *words, size_t nwords)
    __attribute__((alias("gj_bitmap_first_one")));
