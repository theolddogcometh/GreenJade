/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch535: first clear bit in a uint64_t bitmap.
 *
 * Surface (unique symbols):
 *   size_t gj_bitmap_first_zero(const uint64_t *words, size_t nwords);
 *     — Return the bit index of the first clear (0) bit in the little-
 *       endian word array words[0..nwords). Bit 0 is the LSB of words[0],
 *       bit 63 is the MSB of words[0], bit 64 is the LSB of words[1],
 *       and so on. If every bit in the range is set, return nwords*64.
 *       NULL words or nwords == 0 → 0.
 *   size_t __gj_bitmap_first_zero  (alias)
 *   __libcgj_batch535_marker = "libcgj-batch535"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins required (manual trailing-zero scan).
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch535_marker[] = "libcgj-batch535";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Index of the least-significant set bit in w (w must be non-zero).
 * Pure loop — no __builtin_ctzll / ffs.
 */
static size_t
b535_ctz64(uint64_t w)
{
	size_t b;

	for (b = 0u; b < 64u; b++) {
		if ((w & (UINT64_C(1) << b)) != 0u) {
			return b;
		}
	}
	return 0u; /* unreachable when w != 0 */
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitmap_first_zero — first clear bit index in a word bitmap.
 *
 * words:  array of nwords 64-bit limbs; bit i lives in words[i/64]
 *         at shift (i % 64). May be NULL only when treated as empty.
 * nwords: number of limbs; 0 is empty.
 *
 * Returns:
 *   0              if words == NULL or nwords == 0
 *   bit index      of the first 0-bit in [0, nwords*64)
 *   nwords * 64    if every bit in the range is 1
 *
 * Does not call libc or builtins. Does not write through words.
 */
size_t
gj_bitmap_first_zero(const uint64_t *words, size_t nwords)
{
	size_t i;
	uint64_t w;
	uint64_t inv;

	if (words == NULL || nwords == 0u) {
		return 0u;
	}

	for (i = 0u; i < nwords; i++) {
		w = words[i];
		/* All-ones limb has no clear bit; skip. */
		if (w == UINT64_C(0xFFFFFFFFFFFFFFFF)) {
			continue;
		}
		inv = ~w;
		return (i * 64u) + b535_ctz64(inv);
	}

	return nwords * 64u;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_bitmap_first_zero(const uint64_t *words, size_t nwords)
    __attribute__((alias("gj_bitmap_first_zero")));
