/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch574 (renamed surface to avoid multi-def with earlier batch): least common multiple of two uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_euclid_lcm_u64(uint64_t a, uint64_t b);
 *     — Least common multiple via (a / gcd(a, b)) * b with overflow-safe
 *       multiply. lcm(a, 0) / lcm(0, b) / lcm(0, 0) → 0. Returns 0 if the
 *       mathematical LCM does not fit in uint64_t.
 *   uint64_t __gj_euclid_lcm_u64  (alias)
 *   __libcgj_batch574_marker = "libcgj-batch574"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Local static Euclidean GCD; overflow-checked multiply.
 * No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch574_marker[] = "libcgj-batch574";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Euclidean GCD on uint64_t. Local to this TU (not exported).
 * gcd(0, 0) → 0; gcd(x, 0) → x; gcd(0, y) → y.
 */
static uint64_t
b574_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0u) {
		uint64_t u64T = u64A % u64B;
		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/*
 * Multiply a * b into *pOut if the product fits in uint64_t.
 * Returns 0 on success, -1 on overflow.
 */
static int
b574_umul_overflow(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
{
	if (u64A == 0u || u64B == 0u) {
		*pOut = 0u;
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
 * gj_euclid_lcm_u64 — least common multiple of two unsigned 64-bit integers.
 *
 * Computed as (a / gcd(a, b)) * b after reducing a by the GCD so the
 * intermediate product is as small as possible. Returns 0 when either
 * argument is 0, or when the true LCM exceeds UINT64_MAX.
 */
uint64_t
gj_euclid_lcm_u64(uint64_t a, uint64_t b)
{
	uint64_t u64G;
	uint64_t u64Prod;

	if (a == 0u || b == 0u) {
		return 0u;
	}

	u64G = b574_gcd(a, b);
	/* a / g is exact and <= a; multiply by b with overflow check. */
	if (b574_umul_overflow(a / u64G, b, &u64Prod) != 0) {
		return 0u;
	}
	return u64Prod;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_euclid_lcm_u64(uint64_t a, uint64_t b)
    __attribute__((alias("gj_euclid_lcm_u64")));
