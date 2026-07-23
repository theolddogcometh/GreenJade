/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2718: freestanding uint64 align-up (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_align_up_u(uint64_t x, uint64_t a);
 *     - Round x up to the next multiple of a.
 *       a == 0 or a == 1 → return x.
 *       If a is a power of two: mask form (x + a - 1) & ~(a - 1).
 *       Otherwise: general multiple form.
 *       On arithmetic overflow of the round-up, returns 0.
 *   uint64_t __gj_u64_align_up_u  (alias)
 *   __libcgj_batch2718_marker = "libcgj-batch2718"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_align_up (batch526), gj_align_up_u64 (batch375), and
 * gj_u32_align_up_u (batch2716) — u64 u-suffix surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2718_marker[] = "libcgj-batch2718";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Round x up to a multiple of a. a <= 1 → identity.
 * Overflow of the rounded value → 0.
 */
static uint64_t
b2718_align_up(uint64_t u64X, uint64_t u64A)
{
	uint64_t u64Mask;
	uint64_t u64Adj;
	uint64_t u64Rem;

	if (u64A <= 1ull) {
		return u64X;
	}

	/* Power-of-two fast path. */
	if ((u64A & (u64A - 1ull)) == 0ull) {
		u64Mask = u64A - 1ull;
		if (u64X > (UINT64_MAX - u64Mask)) {
			return 0ull;
		}
		u64Adj = (u64X + u64Mask) & ~u64Mask;
		return u64Adj;
	}

	/* General multiple: x + (a - x % a) when not already aligned. */
	u64Rem = u64X % u64A;
	if (u64Rem == 0ull) {
		return u64X;
	}
	if (u64X > (UINT64_MAX - (u64A - u64Rem))) {
		return 0ull;
	}
	return u64X + (u64A - u64Rem);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_align_up_u - round x up to a multiple of a.
 *
 * x: value to align
 * a: alignment / stride (0 or 1 → identity; pow2 uses mask path)
 *
 * Overflow (rounded value would wrap) → 0.
 */
uint64_t
gj_u64_align_up_u(uint64_t u64X, uint64_t u64A)
{
	(void)NULL;
	return b2718_align_up(u64X, u64A);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_align_up_u(uint64_t u64X, uint64_t u64A)
    __attribute__((alias("gj_u64_align_up_u")));
