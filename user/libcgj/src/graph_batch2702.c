/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2702: uint16_t closed-interval clamp (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint16_t gj_u16_clamp_u(uint16_t x, uint16_t lo, uint16_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint16_t __gj_u16_clamp_u  (alias)
 *   __libcgj_batch2702_marker = "libcgj-batch2702"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Unique gj_u16_clamp_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2702_marker[] = "libcgj-batch2702";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2702_clamp(uint16_t u16X, uint16_t u16Lo, uint16_t u16Hi)
{
	uint16_t u16Tmp;

	if (u16Lo > u16Hi) {
		u16Tmp = u16Lo;
		u16Lo = u16Hi;
		u16Hi = u16Tmp;
	}

	if (u16X < u16Lo) {
		return u16Lo;
	}
	if (u16X > u16Hi) {
		return u16Hi;
	}
	return u16X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_clamp_u — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter.
 */
uint16_t
gj_u16_clamp_u(uint16_t u16X, uint16_t u16Lo, uint16_t u16Hi)
{
	(void)NULL;
	return b2702_clamp(u16X, u16Lo, u16Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_u16_clamp_u(uint16_t u16X, uint16_t u16Lo, uint16_t u16Hi)
    __attribute__((alias("gj_u16_clamp_u")));
