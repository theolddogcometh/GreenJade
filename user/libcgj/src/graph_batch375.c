/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch375: power-of-two alignment roundup (uint64_t).
 *
 * Surface (unique symbols):
 *   uint64_t gj_align_up_u64(uint64_t x, uint64_t a);
 *     — Round x up to the nearest multiple of a. a must be a power of
 *       two (including a == 1). When a == 0, returns x unchanged
 *       (degenerate; 0 is not a power of two). When x is already
 *       aligned, returns x. On overflow of the intermediate (x + a - 1),
 *       returns 0.
 *   uint64_t __gj_align_up_u64  (alias)
 *   __libcgj_batch375_marker = "libcgj-batch375"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch375_marker[] = "libcgj-batch375";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_up_u64 — (x + a - 1) & ~(a - 1) with overflow → 0.
 *
 * a is specified as a power of two so the mask form is exact.
 * a <= 1 is a no-op (a == 1 aligns to 1; a == 0 treated as identity).
 */
uint64_t
gj_align_up_u64(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;
	uint64_t u64Adj;

	if (u64A <= 1u) {
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

uint64_t __gj_align_up_u64(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_align_up_u64")));
