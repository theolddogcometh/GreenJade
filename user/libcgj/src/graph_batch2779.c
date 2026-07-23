/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2779: uint64 LCM with overflow -> 0
 * (exclusive continuum wave 2771-2780).
 *
 * Surface (unique symbols):
 *   uint64_t gj_lcm_u64_u(uint64_t a, uint64_t b);
 *     - (a/gcd)*b. Either arg 0 -> 0. Overflow of product -> 0.
 *   uint64_t __gj_lcm_u64_u  (alias)
 *   __libcgj_batch2779_marker = "libcgj-batch2779"
 *
 * Distinct from gj_lcm_u64 (batch229) / gj_lcm_u32_u (batch2778).
 * Unique gj_lcm_u64_u surface only; no multi-def. Local static GCD
 * (does not call gj_gcd_u64_u). Portable 64-bit mul overflow check
 * (no __int128).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2779_marker[] = "libcgj-batch2779";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Euclidean GCD on uint64_t. Local to this TU (not exported).
 * gcd(0, 0) -> 0; gcd(x, 0) -> x; gcd(0, y) -> y.
 */
static uint64_t
b2779_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0ull) {
		uint64_t u64T = u64A % u64B;

		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/*
 * Multiply a * b into *pOut if the product fits in uint64_t.
 * Returns 0 on success, -1 on overflow (or if pOut is NULL).
 * Portable: division check only; no __int128.
 */
static int
b2779_umul_overflow(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	if (pOut == NULL) {
		return -1;
	}
	if (u64A == 0ull || u64B == 0ull) {
		*pOut = 0ull;
		return 0;
	}
	/* a * b overflows iff a > floor(UINT64_MAX / b). */
	if (u64A > (UINT64_C(0xffffffffffffffff) / u64B)) {
		return -1;
	}
	*pOut = u64A * u64B;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lcm_u64_u - least common multiple; overflow or zero input -> 0.
 *
 * Computed as (a / gcd(a, b)) * b after reducing a by the GCD so the
 * intermediate product is as small as possible. Returns 0 when either
 * argument is 0, or when the true LCM exceeds UINT64_MAX. Portable
 * mul path; no __int128. Does not call libc.
 */
uint64_t
gj_lcm_u64_u(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64G;
	uint64_t u64Prod;

	(void)NULL;
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}

	u64G = b2779_gcd(u64A, u64B);
	/* a / g is exact and <= a; multiply by b with overflow check. */
	if (b2779_umul_overflow(u64A / u64G, u64B, &u64Prod) != 0) {
		return 0ull;
	}
	return u64Prod;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_lcm_u64_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_lcm_u64_u")));
