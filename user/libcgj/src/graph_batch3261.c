/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3261: scale uint64_t by unsigned Q16.16 (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_scale_q16_u(uint64_t x, uint32_t s);
 *     - Return floor((x * s) / 2^16) where s is an unsigned Q16.16
 *       scale factor (1.0 == 0x00010000). s==0 → 0; s==0x10000 → x.
 *       If the mathematical result exceeds UINT64_MAX, saturates to
 *       UINT64_MAX. No __int128 (32-bit limb product).
 *   uint64_t __gj_u64_scale_q16_u  (alias)
 *   __libcgj_batch3261_marker = "libcgj-batch3261"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u64_scale_q16_u surface only; no multi-def. Distinct from
 * gj_u64_lerp_q16_u (batch2649) / gj_q16_mul_u (batch3059). No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3261_marker[] = "libcgj-batch3261";

/* Q16.16 unit (1.0). */
#define B3261_Q16_ONE  ((uint32_t)0x00010000u)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((x * s) / 2^16) via a 96-bit limb product, then >> 16.
 * Saturates to UINT64_MAX when the shifted product exceeds 64 bits.
 */
static uint64_t
b3261_scale_q16(uint64_t u64X, uint32_t u32S)
{
	uint64_t u64Xl;
	uint64_t u64Xh;
	uint64_t u64P0;
	uint64_t u64P1;
	uint64_t u64Mid;
	uint64_t u64Lo;
	uint64_t u64Hi;

	if (u32S == 0u) {
		return 0ull;
	}
	if (u32S == B3261_Q16_ONE) {
		return u64X;
	}

	u64Xl = u64X & 0xffffffffull;
	u64Xh = u64X >> 32;
	u64P0 = u64Xl * (uint64_t)u32S;
	u64P1 = u64Xh * (uint64_t)u32S;

	/* product = p0 + (p1 << 32) as 96-bit (hi:lo). */
	u64Mid = (u64P0 >> 32) + (u64P1 & 0xffffffffull);
	u64Lo = (u64P0 & 0xffffffffull) | (u64Mid << 32);
	u64Hi = (u64P1 >> 32) + (u64Mid >> 32);

	/* result = product >> 16; need hi fits in 16 bits else saturate. */
	if (u64Hi > 0xffffull) {
		return UINT64_MAX;
	}
	return (u64Lo >> 16) | (u64Hi << 48);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_scale_q16_u - scale x by unsigned Q16.16 factor s.
 *
 * x: value to scale
 * s: Q16.16 scale (1.0 == 0x00010000)
 *
 * Returns floor((x * s) / 65536), saturating to UINT64_MAX on overflow.
 * Self-contained; no parent wires. No __int128.
 */
uint64_t
gj_u64_scale_q16_u(uint64_t u64X, uint32_t u32S)
{
	(void)NULL;
	return b3261_scale_q16(u64X, u32S);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_scale_q16_u(uint64_t u64X, uint32_t u32S)
    __attribute__((alias("gj_u64_scale_q16_u")));
