/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3504: overflow-safe LCM on uint64_t (_safe_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_lcm_safe_u(uint64_t a, uint64_t b);
 *     — least common multiple via (a / gcd) * b with overflow check.
 *       lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0.
 *       If the mathematical LCM exceeds UINT64_MAX, returns 0 (safe).
 *   uint64_t __gj_u64_lcm_safe_u  (alias)
 *   __libcgj_batch3504_marker = "libcgj-batch3504"
 *
 * Post-3500 modular/number-theory exclusive wave (3501–3510). Distinct
 * from gj_lcm_u64 (batch229), gj_u64_lcm_sat (batch2038), and
 * gj_u64_lcm_sat_u (batch2502) — unique gj_u64_lcm_safe_u name; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3504_marker[] = "libcgj-batch3504";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3504_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ull) {
		uint64_t u64T = u64A % u64B;
		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/*
 * Soft mul with overflow detect. Returns 0 and sets *pOf=1 on overflow;
 * otherwise writes product and returns it with *pOf=0.
 */
static uint64_t
b3504_mul_safe(uint64_t u64A, uint64_t u64B, int *pOf)
{
	if (u64A == 0ull || u64B == 0ull) {
		*pOf = 0;
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		*pOf = 1;
		return 0ull;
	}
	*pOf = 0;
	return u64A * u64B;
}

/*
 * lcm(a, b) = (a / gcd(a, b)) * b; overflow → 0.
 * Zero operand → 0.
 */
static uint64_t
b3504_lcm_safe(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64G;
	uint64_t u64P;
	int iOf;

	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	u64G = b3504_gcd(u64A, u64B);
	/* a / g is exact and <= a; multiply by b with soft overflow check. */
	u64P = b3504_mul_safe(u64A / u64G, u64B, &iOf);
	if (iOf != 0) {
		return 0ull;
	}
	return u64P;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_lcm_safe_u — overflow-safe least common multiple of two uint64_t.
 *
 * a, b: operands (either 0 → 0; overflow → 0)
 */
uint64_t
gj_u64_lcm_safe_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3504_lcm_safe(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_lcm_safe_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_lcm_safe_u")));
