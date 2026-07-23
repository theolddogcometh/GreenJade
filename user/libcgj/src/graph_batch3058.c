/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3058: clamp unsigned Q16.16 to unit interval.
 *
 * Surface (unique symbols):
 *   uint32_t gj_clamp01_q16_u(uint32_t x);
 *     - Confine an unsigned Q16.16 value to [0, 1.0] where 1.0 ==
 *       0x00010000. Values above ONE clamp to ONE; unsigned domain so
 *       the lower bound is always 0.
 *   uint32_t __gj_clamp01_q16_u  (alias)
 *   __libcgj_batch3058_marker = "libcgj-batch3058"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique
 * gj_clamp01_q16_u surface only; no parent wires, no multi-def.
 * Distinct from gj_clamp_u64 (batch587) / gj_clamp_i64 (batch376).
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3058_marker[] = "libcgj-batch3058";

/* Q16.16 unit: 1.0 == 65536. */
#define B3058_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3058_clamp01(uint32_t u32X)
{
	if (u32X > B3058_Q16_ONE) {
		return B3058_Q16_ONE;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clamp01_q16_u - clamp unsigned Q16.16 into the closed unit interval.
 *
 * x: value in Q16.16 units (1.0 == 0x00010000)
 *
 * Returns 0 when x is 0, ONE when x >= ONE, otherwise x unchanged.
 * Self-contained; no parent wires.
 */
uint32_t
gj_clamp01_q16_u(uint32_t u32X)
{
	(void)NULL;
	return b3058_clamp01(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_clamp01_q16_u(uint32_t u32X)
    __attribute__((alias("gj_clamp01_q16_u")));
