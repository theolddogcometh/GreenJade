/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1527: bitmap allocate first free bit (find+set).
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_alloc_first_zero(uint64_t *words, size_t nwords);
 *     — Find the first clear (0) bit in words[0..nwords), set it to 1,
 *       and return its bit index. Bit 0 is LSB of words[0]. If every
 *       bit is already set, or words is NULL / nwords == 0, return
 *       nwords*64 (or 0 when nwords == 0 / NULL).
 *   size_t __gj_bitmap_alloc_first_zero  (alias)
 *   __libcgj_batch1527_marker = "libcgj-batch1527"
 *
 * Distinct from gj_bitmap_first_zero (batch535, read-only scan).
 * Pair with gj_bitmap_free_bit (batch1528). Unique surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins required (manual trailing-zero scan).
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1527_marker[] = "libcgj-batch1527";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of the least-significant set bit in w (w must be non-zero).
 * Pure loop — no __builtin_ctzll / ffs.
 */
static size_t
b1527_ctz64(uint64_t u64W)
{
	size_t uB;

	for (uB = 0u; uB < 64u; uB++) {
		if ((u64W & (UINT64_C(1) << uB)) != 0u) {
			return uB;
		}
	}
	return 0u; /* unreachable when w != 0 */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_alloc_first_zero — find first free bit and mark it allocated.
 *
 * pWords:  mutable array of nwords 64-bit limbs
 * cWords:  number of limbs
 *
 * Returns:
 *   0                 if pWords == NULL or cWords == 0
 *   bit index         of the first 0-bit (now set to 1)
 *   cWords * 64       if every bit in the range was already 1
 */
size_t
gj_bitmap_alloc_first_zero(uint64_t *pWords, size_t cWords)
{
	size_t iWord;
	uint64_t u64W;
	uint64_t u64Inv;
	size_t uBit;
	size_t uIdx;

	if (pWords == NULL || cWords == 0u) {
		return 0u;
	}

	for (iWord = 0u; iWord < cWords; iWord++) {
		u64W = pWords[iWord];
		/* All-ones limb has no free bit; skip. */
		if (u64W == UINT64_C(0xFFFFFFFFFFFFFFFF)) {
			continue;
		}
		u64Inv = ~u64W;
		uBit = b1527_ctz64(u64Inv);
		uIdx = (iWord * 64u) + uBit;
		/* Set the free bit. */
		pWords[iWord] = u64W | (UINT64_C(1) << uBit);
		return uIdx;
	}

	return cWords * 64u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_alloc_first_zero(uint64_t *pWords, size_t cWords)
    __attribute__((alias("gj_bitmap_alloc_first_zero")));
