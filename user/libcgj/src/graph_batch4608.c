/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4608: closed-interval clamp for tick values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_tick_clamp_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint64_t __gj_tick_clamp_u  (alias)
 *   __libcgj_batch4608_marker = "libcgj-batch4608"
 *
 * Exclusive continuum CREATE-ONLY (4601-4610). Distinct from
 * gj_u64_clamp_u (batch2607) / gj_clamp_u64 (batch587) — unique
 * gj_tick_clamp_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4608_marker[] = "libcgj-batch4608";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4608_clamp(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
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
 * gj_tick_clamp_u - confine a tick value to [lo, hi] (order-tolerant).
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Self-contained; no parent wires.
 */
uint64_t
gj_tick_clamp_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b4608_clamp(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_tick_clamp_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_tick_clamp_u")));
