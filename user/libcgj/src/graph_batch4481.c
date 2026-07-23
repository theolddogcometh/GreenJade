/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4481: uint32 multiply-then-divide (rational
 * wave; unique surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mul_div_u2(uint32_t a, uint32_t b, uint32_t d);
 *     - Return floor((a * b) / d) via a uint64_t intermediate product.
 *       d==0 → 0.
 *   uint32_t __gj_u32_mul_div_u2  (alias)
 *   __libcgj_batch4481_marker = "libcgj-batch4481"
 *
 * Exclusive continuum CREATE-ONLY (4481-4490: mul_div_u2, scale_u,
 * u64_mul_div_u2, percent_u, basis_points_u, lerp_u, inv_lerp_u2,
 * smoothstep_tmax_u, clamp01_frac_u, batch_id_4490). Float-free
 * rational unique wave. Distinct from gj_u32_mul_div_u (batch3051) —
 * unique _u2 surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4481_marker[] = "libcgj-batch4481";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor((a * b) / d) with a full-width product in uint64_t.
 * Division by zero soft-returns 0 (no errno; freestanding-safe).
 */
static uint32_t
b4481_mul_div(uint32_t u32A, uint32_t u32B, uint32_t u32D)
{
	uint64_t u64Prod;

	if (u32D == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)u32A * (uint64_t)u32B;
	return (uint32_t)(u64Prod / (uint64_t)u32D);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mul_div_u2 - portable floor((a * b) / d) for uint32_t.
 *
 * a, b: factors
 * d:    divisor (d==0 yields 0)
 *
 * The product is formed in uint64_t so the multiply cannot wrap before
 * the divide. Result is truncated toward zero (unsigned). No __int128.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_mul_div_u2(uint32_t u32A, uint32_t u32B, uint32_t u32D)
{
	(void)NULL;
	return b4481_mul_div(u32A, u32B, u32D);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mul_div_u2(uint32_t u32A, uint32_t u32B, uint32_t u32D)
    __attribute__((alias("gj_u32_mul_div_u2")));
