/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4482: uint32 rational scale (x * num / den).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_scale_u(uint32_t x, uint32_t num, uint32_t den);
 *     - Return floor((x * num) / den) via a uint64_t intermediate.
 *       den==0 → 0.
 *   uint32_t __gj_u32_scale_u  (alias)
 *   __libcgj_batch4482_marker = "libcgj-batch4482"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_scale (batch1208 array)
 * and gj_u32_scale_q16_u (batch3259) — unique gj_u32_scale_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4482_marker[] = "libcgj-batch4482";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((x * num) / den). den==0 soft-returns 0.
 */
static uint32_t
b4482_scale(uint32_t u32X, uint32_t u32Num, uint32_t u32Den)
{
	uint64_t u64Prod;

	if (u32Den == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32X * (uint64_t)u32Num;
	return (uint32_t)(u64Prod / (uint64_t)u32Den);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_scale_u - scale x by the rational num/den (floor).
 *
 * x:   value to scale
 * num: numerator
 * den: denominator (den==0 yields 0)
 *
 * Product formed in uint64_t. Self-contained; no parent wires.
 */
uint32_t
gj_u32_scale_u(uint32_t u32X, uint32_t u32Num, uint32_t u32Den)
{
	(void)NULL;
	return b4482_scale(u32X, u32Num, u32Den);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_scale_u(uint32_t u32X, uint32_t u32Num, uint32_t u32Den)
    __attribute__((alias("gj_u32_scale_u")));
