/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3802: uint64_t closed-interval clip (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clip_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint64_t __gj_u64_clip_u  (alias)
 *   __libcgj_batch3802_marker = "libcgj-batch3802"
 *
 * Exclusive continuum CREATE-ONLY wave (3801-3810). Unique
 * gj_u64_clip_u surface only; no multi-def. Distinct from
 * gj_u32_clip_u (batch3801), gj_clamp_u64 (batch587), and
 * gj_u32_clamp_u (batch2703). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3802_marker[] = "libcgj-batch3802";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clip x into [min(lo,hi), max(lo,hi)]. */
static uint64_t
b3802_clip(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Tmp;

	if (u64Lo > u64Hi) {
		u64Tmp = u64Lo;
		u64Lo = u64Hi;
		u64Hi = u64Tmp;
	}
	if (u64X < u64Lo) {
		return u64Lo;
	}
	if (u64X > u64Hi) {
		return u64Hi;
	}
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clip_u - confine x to the closed interval [lo, hi].
 *
 * x:  value to clip
 * lo: lower bound (may be greater than hi; reordered)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter. No parent wires.
 */
uint64_t
gj_u64_clip_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b3802_clip(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clip_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_clip_u")));
