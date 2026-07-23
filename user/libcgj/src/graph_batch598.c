/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch598: next uint64 bit permutation (same popcount).
 *
 * Surface (unique symbols):
 *   uint64_t gj_next_perm_bits_u64(uint64_t v);
 *     — Lexicographically next larger integer with the same number of
 *       1-bits as v (Gosper's hack / "snoob"). Returns 0 when v == 0
 *       or when no larger same-popcount value exists in 64 bits
 *       (unsigned overflow of the ripple step).
 *   uint64_t __gj_next_perm_bits_u64  (alias)
 *   __libcgj_batch598_marker = "libcgj-batch598"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch598_marker[] = "libcgj-batch598";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_next_perm_bits_u64 — next combination of the same popcount.
 *
 * v: current bit mask (any uint64_t)
 *
 * Gosper:
 *   c = isolate lowest set bit of v
 *   r = v + c          (ripples the trailing ones into the next zero)
 *   ones = ((r ^ v) >> 2) / c   (re-place remaining ones right of r)
 *   return r | ones
 *
 * v == 0 has no lowest set bit (c would be 0); defined to return 0.
 * If v + c overflows 64 bits, there is no next same-popcount word; 0.
 */
uint64_t
gj_next_perm_bits_u64(uint64_t v)
{
	uint64_t c;
	uint64_t r;

	if (v == 0ull) {
		return 0ull;
	}

	/* Isolate lowest set bit: v & -v (unsigned wrap of -v is defined). */
	c = v & (uint64_t)(0ull - v);
	r = v + c;
	if (r < v) {
		/* Ripple overflowed: last combination of this popcount. */
		return 0ull;
	}

	/* c is a power of two; (r ^ v) holds the bits that changed. */
	return (((r ^ v) >> 2) / c) | r;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_next_perm_bits_u64(uint64_t v)
    __attribute__((alias("gj_next_perm_bits_u64")));
