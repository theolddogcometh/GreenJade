/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2504: saturating LCM on uint32_t (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lcm_sat_u(uint32_t a, uint32_t b);
 *     — least common multiple via (a / gcd) * b with soft mul sat.
 *       lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0.
 *       If the mathematical LCM exceeds UINT32_MAX, returns UINT32_MAX.
 *   uint32_t __gj_u32_lcm_sat_u  (alias)
 *   __libcgj_batch2504_marker = "libcgj-batch2504"
 *
 * Post-2500 number-theory exclusive wave (2501–2510). Unique
 * gj_u32_lcm_sat_u surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2504_marker[] = "libcgj-batch2504";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2504_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;
		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/* Soft saturating mul; returns UINT32_MAX on overflow. */
static uint32_t
b2504_mul_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		return UINT32_MAX;
	}
	return u32A * u32B;
}

/*
 * lcm(a, b) = (a / gcd(a, b)) * b with saturation.
 * Zero operand → 0.
 */
static uint32_t
b2504_lcm_sat(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32G;

	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	u32G = b2504_gcd(u32A, u32B);
	/* a / g is exact and <= a; multiply by b with soft sat. */
	return b2504_mul_sat(u32A / u32G, u32B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lcm_sat_u — saturating least common multiple of two uint32_t.
 *
 * a, b: operands (either 0 → 0; overflow → UINT32_MAX)
 */
uint32_t
gj_u32_lcm_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2504_lcm_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lcm_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_lcm_sat_u")));
