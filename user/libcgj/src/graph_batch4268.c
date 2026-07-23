/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4268: freestanding uint32 align-down (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_align_down_u2(uint32_t x, uint32_t a);
 *     - Round x down to the nearest multiple of a.
 *       a == 0 → return x (identity).
 *       a is assumed a power of two; if not a power of two, treat as
 *       a = 1 (align-to-1 is identity → return x).
 *       Power-of-two path: x & ~(a - 1).
 *   uint32_t __gj_u32_align_down_u2  (alias)
 *   __libcgj_batch4268_marker = "libcgj-batch4268"
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

const char __libcgj_batch4268_marker[] = "libcgj-batch4268";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x down to a multiple of a.
 * a == 0 or non-power-of-two → identity (a treated as 1).
 */
static uint32_t
b4268_align_down(uint32_t u32X, uint32_t u32A)
{
	uint32_t u32Mask;

	if (u32A == 0u) {
		return u32X;
	}

	/* Non-power-of-two → treat as a = 1 (identity). */
	if ((u32A & (u32A - 1u)) != 0u) {
		return u32X;
	}

	u32Mask = u32A - 1u;
	return u32X & ~u32Mask;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_align_down_u2 - round x down to power-of-two alignment a.
 *
 * x: value to align
 * a: alignment (0 → identity; non-pow2 treated as a=1 → identity)
 *
 * No parent wires.
 */
uint32_t
gj_u32_align_down_u2(uint32_t u32X, uint32_t u32A)
{
	(void)NULL;
	return b4268_align_down(u32X, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_align_down_u2(uint32_t u32X, uint32_t u32A)
    __attribute__((alias("gj_u32_align_down_u2")));
