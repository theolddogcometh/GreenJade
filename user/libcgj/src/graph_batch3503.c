/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3503: overflow-safe LCM on uint32_t (_safe_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_lcm_safe_u(uint32_t a, uint32_t b);
 *     — least common multiple via (a / gcd) * b with overflow check.
 *       lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0.
 *       If the mathematical LCM exceeds UINT32_MAX, returns 0 (safe).
 *   uint32_t __gj_u32_lcm_safe_u  (alias)
 *   __libcgj_batch3503_marker = "libcgj-batch3503"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Distinct
 * from gj_u32_lcm_sat_u (batch2504, UINT32_MAX on overflow) — unique
 * gj_u32_lcm_safe_u name; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3503_marker[] = "libcgj-batch3503";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3503_gcd(uint32_t u32A, uint32_t u32B)
{
	while (u32B != 0u) {
		uint32_t u32T = u32A % u32B;
		u32A = u32B;
		u32B = u32T;
	}
	return u32A;
}

/*
 * Soft mul with overflow detect. Returns 0 and sets *pOf=1 on overflow;
 * otherwise writes product and returns it with *pOf=0.
 */
static uint32_t
b3503_mul_safe(uint32_t u32A, uint32_t u32B, int *pOf)
{
	if (u32A == 0u || u32B == 0u) {
		*pOf = 0;
		return 0u;
	}
	if (u32A > (UINT32_MAX / u32B)) {
		*pOf = 1;
		return 0u;
	}
	*pOf = 0;
	return u32A * u32B;
}

/*
 * lcm(a, b) = (a / gcd(a, b)) * b; overflow → 0.
 * Zero operand → 0.
 */
static uint32_t
b3503_lcm_safe(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32G;
	uint32_t u32P;
	int iOf;

	if (u32A == 0u || u32B == 0u) {
		return 0u;
	}
	u32G = b3503_gcd(u32A, u32B);
	/* a / g is exact and <= a; multiply by b with soft overflow check. */
	u32P = b3503_mul_safe(u32A / u32G, u32B, &iOf);
	if (iOf != 0) {
		return 0u;
	}
	return u32P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_lcm_safe_u — overflow-safe least common multiple of two uint32_t.
 *
 * a, b: operands (either 0 → 0; overflow → 0)
 */
uint32_t
gj_u32_lcm_safe_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3503_lcm_safe(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_lcm_safe_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_lcm_safe_u")));
