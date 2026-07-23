/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch527: power-of-two alignment round-down (uint64_t).
 *
 * Surface (unique symbols):
 *   uint64_t gj_align_down(uint64_t x, uint64_t a);
 *     — Round x down to the nearest multiple of a. a must be a power of
 *       two and non-zero. When a == 0 or a is not a power of two,
 *       returns x unchanged. When x is already aligned, returns x.
 *   uint64_t __gj_align_down  (alias)
 *   __libcgj_batch527_marker = "libcgj-batch527"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch527_marker[] = "libcgj-batch527";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_align_down — x & ~(a - 1) when a is a non-zero power of two.
 *
 * a is specified as a power of two so the mask form is exact.
 * a == 0 or non-power-of-two is treated as identity (return x).
 * a == 1 is a power of two and is a no-op (align to 1).
 */
uint64_t
gj_align_down(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;

	/* Reject zero and non-powers-of-two: (a & (a - 1)) != 0. */
	if (u64A == 0u || (u64A & (u64A - 1u)) != 0u) {
		return u64X;
	}

	u64Mask = u64A - 1u;
	return u64X & ~u64Mask;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_align_down(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_align_down")));
