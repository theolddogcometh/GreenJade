/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4262: freestanding uint64 align-down (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_align_down_u2(uint64_t x, uint64_t a);
 *     - Round x down to the nearest multiple of a.
 *       a == 0 → return x (identity).
 *       a is assumed a power of two; if not a power of two, treat as
 *       a = 1 (align-to-1 is identity → return x).
 *       Power-of-two path: x & ~(a - 1).
 *   uint64_t __gj_u64_align_down_u2  (alias)
 *   __libcgj_batch4262_marker = "libcgj-batch4262"
 *
 * Exclusive continuum CREATE-ONLY (4261-4270): align/page wave.
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4262_marker[] = "libcgj-batch4262";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x down to a multiple of a.
 * a == 0 or non-power-of-two → identity (a treated as 1).
 */
static uint64_t
b4262_align_down(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;

	if (u64A == 0ull) {
		return u64X;
	}

	/* Non-power-of-two → treat as a = 1 (identity). */
	if ((u64A & (u64A - 1ull)) != 0ull) {
		return u64X;
	}

	u64Mask = u64A - 1ull;
	return u64X & ~u64Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_align_down_u2 - round x down to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (0 → identity; non-pow2 treated as a=1 → identity)
 *
 * No parent wires.
 */
uint64_t
gj_u64_align_down_u2(uint64_t u64X, uint64_t u64A)
{
	(void)NULL;
	return b4262_align_down(u64X, u64A);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_align_down_u2(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_u64_align_down_u2")));
