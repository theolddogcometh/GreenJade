/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3055: unsigned uint32 linear interpolation.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lerp_u32_u(uint32_t a, uint32_t b, uint32_t t,
 *                          uint32_t t_max);
 *     - Return a + (b - a) * t / t_max using a 64-bit intermediate for
 *       the multiply. t_max == 0 → a. Handles b < a without underflow.
 *   uint32_t __gj_lerp_u32_u  (alias)
 *   __libcgj_batch3055_marker = "libcgj-batch3055"
 *
 * CREATE-ONLY exclusive continuum wave (3051-3060). Unique
 * gj_lerp_u32_u surface only; no parent wires, no multi-def.
 * Distinct from gj_lerp_i32 (batch959). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3055_marker[] = "libcgj-batch3055";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Unsigned lerp: a + (b-a)*t/t_max, careful when b < a.
 * Product (diff * t) is formed in uint64_t.
 */
static uint32_t
b3055_lerp(uint32_t u32A, uint32_t u32B, uint32_t u32T, uint32_t u32TMax)
{
	uint64_t u64Diff;
	uint64_t u64Num;

	if (u32TMax == 0u) {
		return u32A;
	}
	if (u32B >= u32A) {
		u64Diff = (uint64_t)u32B - (uint64_t)u32A;
		u64Num = u64Diff * (uint64_t)u32T;
		return u32A + (uint32_t)(u64Num / (uint64_t)u32TMax);
	}
	u64Diff = (uint64_t)u32A - (uint64_t)u32B;
	u64Num = u64Diff * (uint64_t)u32T;
	return u32A - (uint32_t)(u64Num / (uint64_t)u32TMax);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lerp_u32_u - unsigned linear interpolation on uint32_t.
 *
 * a, b: endpoints
 * t:    blend parameter (caller-owned range; typically 0..t_max)
 * t_max: blend scale; t_max==0 returns a
 *
 * Truncates toward zero on the division. Self-contained; no parent wires.
 */
uint32_t
gj_lerp_u32_u(uint32_t u32A, uint32_t u32B, uint32_t u32T, uint32_t u32TMax)
{
	(void)NULL;
	return b3055_lerp(u32A, u32B, u32T, u32TMax);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lerp_u32_u(uint32_t u32A, uint32_t u32B, uint32_t u32T,
    uint32_t u32TMax)
    __attribute__((alias("gj_lerp_u32_u")));
