/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2716: freestanding uint32 align-up (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_align_up_u(uint32_t x, uint32_t a);
 *     - Round x up to the next multiple of a.
 *       a == 0 or a == 1 → return x.
 *       If a is a power of two: mask form (x + a - 1) & ~(a - 1).
 *       Otherwise: general multiple form.
 *       On arithmetic overflow of the round-up, returns 0.
 *   uint32_t __gj_u32_align_up_u  (alias)
 *   __libcgj_batch2716_marker = "libcgj-batch2716"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_u32_align_up (batch779) — u-suffix public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2716_marker[] = "libcgj-batch2716";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to a multiple of a. a <= 1 → identity.
 * Overflow of the rounded value → 0.
 */
static uint32_t
b2716_align_up(uint32_t u32X, uint32_t u32A)
{
	uint32_t u32Mask;
	uint32_t u32Adj;
	uint32_t u32Rem;

	if (u32A <= 1u) {
		return u32X;
	}

	/* Power-of-two fast path. */
	if ((u32A & (u32A - 1u)) == 0u) {
		u32Mask = u32A - 1u;
		u32Adj = (u32X + u32Mask) & ~u32Mask;
		if (u32Adj < u32X) {
			return 0u;
		}
		return u32Adj;
	}

	/* General multiple: x + (a - x % a) when not already aligned. */
	u32Rem = u32X % u32A;
	if (u32Rem == 0u) {
		return u32X;
	}
	u32Adj = u32X + (u32A - u32Rem);
	if (u32Adj < u32X) {
		return 0u;
	}
	return u32Adj;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_align_up_u - round x up to a multiple of a.
 *
 * x: value to align
 * a: alignment / stride (0 or 1 → identity; pow2 uses mask path)
 *
 * Overflow (rounded value wraps below x) → 0.
 */
uint32_t
gj_u32_align_up_u(uint32_t u32X, uint32_t u32A)
{
	(void)NULL;
	return b2716_align_up(u32X, u32A);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_align_up_u(uint32_t u32X, uint32_t u32A)
    __attribute__((alias("gj_u32_align_up_u")));
