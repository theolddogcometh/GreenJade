/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4267: freestanding uint32 align-up (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_align_up_u2(uint32_t x, uint32_t a);
 *     - Round x up to the next multiple of a.
 *       a == 0 → return x (identity).
 *       a is assumed a power of two; if not a power of two, treat as
 *       a = 1 (align-to-1 is identity → return x).
 *       Power-of-two path: (x + a - 1) & ~(a - 1).
 *       On overflow of the intermediate (x + a - 1), returns 0.
 *   uint32_t __gj_u32_align_up_u2  (alias)
 *   __libcgj_batch4267_marker = "libcgj-batch4267"
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

const char __libcgj_batch4267_marker[] = "libcgj-batch4267";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to a multiple of a.
 * a == 0 or non-power-of-two → identity (a treated as 1).
 * Overflow of (x + a - 1) → 0.
 */
static uint32_t
b4267_align_up(uint32_t u32X, uint32_t u32A)
{
	uint32_t u32Mask;
	uint32_t u32Adj;

	if (u32A == 0u) {
		return u32X;
	}

	/* Non-power-of-two → treat as a = 1 (identity). */
	if ((u32A & (u32A - 1u)) != 0u) {
		return u32X;
	}

	if (u32A == 1u) {
		return u32X;
	}

	u32Mask = u32A - 1u;
	u32Adj = (u32X + u32Mask) & ~u32Mask;
	if (u32Adj < u32X) {
		return 0u;
	}
	return u32Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_align_up_u2 - round x up to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (0 → identity; non-pow2 treated as a=1 → identity)
 *
 * Overflow of the round-up intermediate → 0. No parent wires.
 */
uint32_t
gj_u32_align_up_u2(uint32_t u32X, uint32_t u32A)
{
	(void)NULL;
	return b4267_align_up(u32X, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_align_up_u2(uint32_t u32X, uint32_t u32A)
    __attribute__((alias("gj_u32_align_up_u2")));
