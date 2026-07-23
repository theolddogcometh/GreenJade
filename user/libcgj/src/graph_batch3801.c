/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3801: uint32_t closed-interval clip (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clip_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint32_t __gj_u32_clip_u  (alias)
 *   __libcgj_batch3801_marker = "libcgj-batch3801"
 *
 * Exclusive continuum CREATE-ONLY wave (3801-3810). Unique
 * gj_u32_clip_u surface only; no multi-def. Distinct from
 * gj_u32_clamp_u (batch2703), gj_u32_clamp3 (batch2358), and
 * gj_u32_clamp_lo_hi_u (batch3455). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3801_marker[] = "libcgj-batch3801";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clip x into [min(lo,hi), max(lo,hi)]. */
static uint32_t
b3801_clip(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	uint32_t u32Tmp;

	if (u32Lo > u32Hi) {
		u32Tmp = u32Lo;
		u32Lo = u32Hi;
		u32Hi = u32Tmp;
	}
	if (u32X < u32Lo) {
		return u32Lo;
	}
	if (u32X > u32Hi) {
		return u32Hi;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clip_u - confine x to the closed interval [lo, hi].
 *
 * x:  value to clip
 * lo: lower bound (may be greater than hi; reordered)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter. No parent wires.
 */
uint32_t
gj_u32_clip_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b3801_clip(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clip_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u32_clip_u")));
