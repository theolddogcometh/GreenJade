/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4857: workgroup-domain closed-interval clamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_clamp_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the range is ordered.
 *   uint32_t __gj_wg_clamp_u  (alias)
 *   __libcgj_batch4857_marker = "libcgj-batch4857"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_clamp_u
 * surface only; no multi-def. Distinct from gj_u32_clamp_u
 * (batch2703) and gj_u32_clamp3 (batch2358) — workgroup-named surface.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4857_marker[] = "libcgj-batch4857";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clamp x into [min(lo,hi), max(lo,hi)]. */
static uint32_t
b4857_clamp(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
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
 * gj_wg_clamp_u - confine a workgroup coordinate into [lo, hi].
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; reordered)
 * hi: upper bound
 *
 * Returns ordered low if below, ordered high if above, else x.
 * Self-contained; no parent wires.
 */
uint32_t
gj_wg_clamp_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b4857_clamp(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_clamp_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_wg_clamp_u")));
