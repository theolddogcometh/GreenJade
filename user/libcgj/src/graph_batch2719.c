/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2719: power-of-two align-down uint64 (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_align_down_u(uint64_t x, uint64_t a);
 *     - Round x down to the nearest multiple of a. a must be a power of
 *       two and non-zero. When a == 0 or a is not a power of two,
 *       returns x unchanged.
 *   uint64_t __gj_u64_align_down_u  (alias)
 *   __libcgj_batch2719_marker = "libcgj-batch2719"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_align_down (batch527) and gj_u32_align_down_u (batch2717) —
 * u64 u-suffix public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2719_marker[] = "libcgj-batch2719";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * x & ~(a - 1) when a is a non-zero power of two.
 * a == 0 or non-power-of-two → identity (return x).
 */
static uint64_t
b2719_align_down(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;

	if (u64A == 0ull || (u64A & (u64A - 1ull)) != 0ull) {
		return u64X;
	}

	u64Mask = u64A - 1ull;
	return u64X & ~u64Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_align_down_u - round x down to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (must be non-zero power of two; else identity)
 */
uint64_t
gj_u64_align_down_u(uint64_t u64X, uint64_t u64A)
{
	(void)NULL;
	return b2719_align_down(u64X, u64A);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_align_down_u(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_u64_align_down_u")));
