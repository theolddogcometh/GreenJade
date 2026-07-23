/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4489: clamp uint32 into fractional unit
 * interval [0, scale].
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clamp01_frac_u(uint32_t x, uint32_t scale);
 *     - Confine x to the closed interval [0, scale]. Values above
 *       scale clamp to scale; the unsigned domain already saturates
 *       the lower bound at 0.
 *   uint32_t __gj_u32_clamp01_frac_u  (alias)
 *   __libcgj_batch4489_marker = "libcgj-batch4489"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_clamp01_u (batch4001)
 * and gj_u32_clamp_u (batch2703) — unique gj_u32_clamp01_frac_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4489_marker[] = "libcgj-batch4489";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4489_clamp01_frac(uint32_t u32X, uint32_t u32Scale)
{
	if (u32X > u32Scale) {
		return u32Scale;
	}
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clamp01_frac_u - clamp uint32_t into [0, scale].
 *
 * x:     value to clamp
 * scale: upper bound of the fractional unit interval
 *
 * Returns 0 when x is 0, scale when x >= scale, otherwise x.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_clamp01_frac_u(uint32_t u32X, uint32_t u32Scale)
{
	(void)NULL;
	return b4489_clamp01_frac(u32X, u32Scale);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clamp01_frac_u(uint32_t u32X, uint32_t u32Scale)
    __attribute__((alias("gj_u32_clamp01_frac_u")));
