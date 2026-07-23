/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2038: saturating LCM on uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_lcm_sat(uint64_t a, uint64_t b);
 *     — least common multiple via (a / gcd) * b with soft mul sat.
 *       lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0.
 *       If the mathematical LCM exceeds UINT64_MAX, returns UINT64_MAX.
 *   uint64_t __gj_u64_lcm_sat  (alias)
 *   __libcgj_batch2038_marker = "libcgj-batch2038"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_lcm_u64 (batch229), which returns 0 on overflow — this surface
 * saturates instead; unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2038_marker[] = "libcgj-batch2038";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2038_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ull) {
		uint64_t u64T = u64A % u64B;
		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/* Soft saturating mul; returns UINT64_MAX on overflow. */
static uint64_t
b2038_mul_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/*
 * lcm(a, b) = (a / gcd(a, b)) * b with saturation.
 * Zero operand → 0.
 */
static uint64_t
b2038_lcm_sat(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64G;

	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	u64G = b2038_gcd(u64A, u64B);
	/* a / g is exact and <= a; multiply by b with soft sat. */
	return b2038_mul_sat(u64A / u64G, u64B);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lcm_sat — saturating least common multiple of two uint64_t values.
 *
 * a, b: operands (either 0 → 0; overflow → UINT64_MAX)
 */
uint64_t
gj_u64_lcm_sat(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2038_lcm_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_lcm_sat(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_lcm_sat")));
