/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3456: uint64_t closed-interval clamp
 * (lo_hi exclusive _u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamp_lo_hi_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint64_t __gj_u64_clamp_lo_hi_u  (alias)
 *   __libcgj_batch3456_marker = "libcgj-batch3456"
 *
 * CREATE-ONLY exclusive continuum wave (3451-3460). Unique
 * gj_u64_clamp_lo_hi_u surface only; no multi-def. Distinct from
 * gj_u64_clamp_u (batch2607), gj_u64_clamp3 (batch2359),
 * gj_u64_clamp_range (batch1371), and gj_u64_clamp_soft_u
 * (batch2704). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3456_marker[] = "libcgj-batch3456";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3456_clamp(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
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
 * gj_u64_clamp_lo_hi_u — confine x to the closed interval [lo, hi].
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter. Does not call libc.
 * No parent wires.
 */
uint64_t
gj_u64_clamp_lo_hi_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b3456_clamp(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamp_lo_hi_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_clamp_lo_hi_u")));
