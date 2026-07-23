/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2701: uint8_t closed-interval clamp (exclusive _u).
 *
 * Surface (unique symbols):
 *   uint8_t gj_u8_clamp_u(uint8_t x, uint8_t lo, uint8_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   uint8_t __gj_u8_clamp_u  (alias)
 *   __libcgj_batch2701_marker = "libcgj-batch2701"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Unique gj_u8_clamp_u surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2701_marker[] = "libcgj-batch2701";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b2701_clamp(uint8_t u8X, uint8_t u8Lo, uint8_t u8Hi)
{
	uint8_t u8Tmp;

	if (u8Lo > u8Hi) {
		u8Tmp = u8Lo;
		u8Lo = u8Hi;
		u8Hi = u8Tmp;
	}

	if (u8X < u8Lo) {
		return u8Lo;
	}
	if (u8X > u8Hi) {
		return u8Hi;
	}
	return u8X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_clamp_u — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter.
 */
uint8_t
gj_u8_clamp_u(uint8_t u8X, uint8_t u8Lo, uint8_t u8Hi)
{
	(void)NULL;
	return b2701_clamp(u8X, u8Lo, u8Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_u8_clamp_u(uint8_t u8X, uint8_t u8Lo, uint8_t u8Hi)
    __attribute__((alias("gj_u8_clamp_u")));
