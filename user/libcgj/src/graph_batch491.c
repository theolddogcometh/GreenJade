/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch491: freestanding unbounded dense bitset
 * set/clear/test over uint64_t words (no nbits bound parameter).
 *
 * Surface (unique symbols — batch118 owns gj_bitset_set/clear/test with
 * nbits bounds, so this TU uses gj_bset_* names):
 *   void gj_bset_set(uint64_t *words, size_t bit);
 *   void gj_bset_clear(uint64_t *words, size_t bit);
 *   int  gj_bset_test(const uint64_t *words, size_t bit);
 *   void __gj_bset_set    (alias)
 *   void __gj_bset_clear  (alias)
 *   int  __gj_bset_test   (alias)
 *   __libcgj_batch491_marker = "libcgj-batch491"
 *
 * Layout: words[bit/64] holds bit (bit%64) LSB-first within the word.
 * No nbits argument — caller owns the array length; out-of-range bits are
 * not checked. NULL words is a no-op (test → 0).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party bitset source copied. Distinct from batch118.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch491_marker[] = "libcgj-batch491";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bset_set — set bit `bit` in the dense word array (LSB-first).
 *
 * words: bitset storage (may be NULL → no-op).
 * bit: absolute bit index; word = bit >> 6, lane = bit & 63.
 */
void
gj_bset_set(uint64_t *words, size_t bit)
{
	if (words == NULL) {
		return;
	}
	words[bit >> 6] |= (uint64_t)1 << (bit & 63u);
}

/*
 * gj_bset_clear — clear bit `bit` in the dense word array (LSB-first).
 */
void
gj_bset_clear(uint64_t *words, size_t bit)
{
	if (words == NULL) {
		return;
	}
	words[bit >> 6] &= ~((uint64_t)1 << (bit & 63u));
}

/*
 * gj_bset_test — return 1 if bit `bit` is set, else 0.
 * NULL words → 0.
 */
int
gj_bset_test(const uint64_t *words, size_t bit)
{
	if (words == NULL) {
		return 0;
	}
	return (int)((words[bit >> 6] >> (bit & 63u)) & (uint64_t)1);
}

void __gj_bset_set(uint64_t *words, size_t bit)
    __attribute__((alias("gj_bset_set")));

void __gj_bset_clear(uint64_t *words, size_t bit)
    __attribute__((alias("gj_bset_clear")));

int __gj_bset_test(const uint64_t *words, size_t bit)
    __attribute__((alias("gj_bset_test")));
