/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4159: clamp unsigned Q16.16 to [0, 1.0] (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_clamp01_u(uint32_t x);
 *     - Clamp x to the closed interval [0, 1 << 16] (Q16.16 unit).
 *       Unsigned domain already bounds the lower end at 0.
 *   uint32_t __gj_q16x_clamp01_u  (alias)
 *   __libcgj_batch4159_marker = "libcgj-batch4159"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_u32_clamp01_u (batch4001) and gj_clamp01_q16_u (batch3058) —
 * unique gj_q16x_clamp01_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4159_marker[] = "libcgj-batch4159";

/* Q16.16 unit (1.0): 1 << 16. */
#define B4159_ONE  ((uint32_t)(1u << 16))

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4159_clamp01 — clamp unsigned Q16.16 into [0, 1.0].
 */
static uint32_t
b4159_clamp01(uint32_t u32X)
{
	if (u32X > B4159_ONE) {
		return B4159_ONE;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_clamp01_u - clamp unsigned Q16.16 into [0, 1 << 16].
 *
 * x: unsigned Q16.16 value
 *
 * Returns x when x <= 0x10000, else 0x10000. Lower bound is inherent
 * for uint32. No parent wires.
 */
uint32_t
gj_q16x_clamp01_u(uint32_t u32X)
{
	(void)NULL;
	return b4159_clamp01(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_clamp01_u(uint32_t u32X)
    __attribute__((alias("gj_q16x_clamp01_u")));
