/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2358: uint32_t closed-interval clamp
 * (post-2350 exclusive surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clamp3(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the range is ordered.
 *   uint32_t __gj_u32_clamp3  (alias)
 *   __libcgj_batch2358_marker = "libcgj-batch2358"
 *
 * Distinct from gj_clamp_u64 (batch587) and gj_u32_clamp_arr
 * (batch1209) - unique gj_u32_clamp3 surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2358_marker[] = "libcgj-batch2358";

/* ---- freestanding helpers ---------------------------------------------- */

/* Clamp x into [min(lo,hi), max(lo,hi)]. */
static uint32_t
b2358_clamp3(uint32_t uX, uint32_t uLo, uint32_t uHi)
{
	uint32_t uTmp;

	if (uLo > uHi) {
		uTmp = uLo;
		uLo = uHi;
		uHi = uTmp;
	}
	if (uX < uLo) {
		return uLo;
	}
	if (uX > uHi) {
		return uHi;
	}
	return uX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clamp3 - confine x to the closed interval [lo, hi].
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; reordered)
 * hi: upper bound
 *
 * Returns lo if below ordered low, hi if above ordered high, else x.
 * Does not call libc.
 */
uint32_t
gj_u32_clamp3(uint32_t uX, uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b2358_clamp3(uX, uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clamp3(uint32_t uX, uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_u32_clamp3")));
