/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4261: freestanding uint64 align-up (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_align_up_u2(uint64_t x, uint64_t a);
 *     - Round x up to the next multiple of a.
 *       a == 0 → return x (identity).
 *       a is assumed a power of two; if not a power of two, treat as
 *       a = 1 (align-to-1 is identity → return x).
 *       Power-of-two path: (x + a - 1) & ~(a - 1).
 *       On overflow of the intermediate (x + a - 1), returns 0.
 *   uint64_t __gj_u64_align_up_u2  (alias)
 *   __libcgj_batch4261_marker = "libcgj-batch4261"
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

const char __libcgj_batch4261_marker[] = "libcgj-batch4261";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to a multiple of a.
 * a == 0 or non-power-of-two → identity (a treated as 1).
 * Overflow of (x + a - 1) → 0.
 */
static uint64_t
b4261_align_up(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;

	if (u64A == 0ull) {
		return u64X;
	}

	/* Non-power-of-two → treat as a = 1 (identity). */
	if ((u64A & (u64A - 1ull)) != 0ull) {
		return u64X;
	}

	/* a == 1 is a power of two and is a no-op. */
	if (u64A == 1ull) {
		return u64X;
	}

	u64Mask = u64A - 1ull;
	if (u64X > (UINT64_MAX - u64Mask)) {
		return 0ull;
	}
	return (u64X + u64Mask) & ~u64Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_align_up_u2 - round x up to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (0 → identity; non-pow2 treated as a=1 → identity)
 *
 * Overflow of the round-up intermediate → 0. No parent wires.
 */
uint64_t
gj_u64_align_up_u2(uint64_t u64X, uint64_t u64A)
{
	(void)NULL;
	return b4261_align_up(u64X, u64A);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_align_up_u2(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_u64_align_up_u2")));
