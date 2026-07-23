/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch526: power-of-two alignment roundup (uint64_t).
 *
 * Surface (unique symbols):
 *   uint64_t gj_align_up(uint64_t x, uint64_t a);
 *     — Round x up to the nearest multiple of a. a must be a non-zero
 *       power of two (including a == 1). When a == 0 or a is not a
 *       power of two, returns x unchanged. When x is already aligned,
 *       returns x. On overflow of the intermediate (x + a - 1),
 *       returns 0.
 *   uint64_t __gj_align_up  (alias)
 *   __libcgj_batch526_marker = "libcgj-batch526"
 *
 * Formula (valid a): (x + (a - 1)) & ~(a - 1)
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch526_marker[] = "libcgj-batch526";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_up — (x + a - 1) & ~(a - 1) with overflow → 0.
 *
 * a must be a non-zero power of two so the mask form is exact.
 * a == 0 or (a & (a - 1)) != 0 → identity (return x).
 */
uint64_t
gj_align_up(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	/* Reject zero and non-power-of-two alignments. */
	if (u64A == 0u || (u64A & (u64A - 1u)) != 0u) {
		return u64X;
	}

	u64Mask = u64A - 1u;

	/* Overflow of (x + (a - 1)) would wrap; signal with 0. */
	if (u64X > (UINT64_MAX - u64Mask)) {
		return 0u;
	}

	u64Adj = (u64X + u64Mask) & ~u64Mask;
	return u64Adj;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_align_up(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_align_up")));
