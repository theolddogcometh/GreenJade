/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch383: n-ary least common multiple for uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_lcm_n_u64(const uint64_t *a, size_t n);
 *     — Least common multiple of a[0..n). Empty input (n == 0 or a ==
 *       NULL) returns 0. Any zero element yields 0 (lcm(*, 0) == 0).
 *       Returns 0 if the mathematical LCM does not fit in uint64_t.
 *       For n == 1 returns a[0] (including 0).
 *   uint64_t __gj_lcm_n_u64  (alias)
 *   __libcgj_batch383_marker = "libcgj-batch383"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Local static Euclidean GCD; overflow-checked multiply.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch383_marker[] = "libcgj-batch383";

/* ---- helpers ----------------------------------------------------------- */

/*
 * Euclidean GCD on uint64_t. Local to this TU (not exported).
 * gcd(0, 0) → 0; gcd(x, 0) → x; gcd(0, y) → y.
 */
static uint64_t
b383_gcd(uint64_t u64A, uint64_t u64B)
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
b383_umul_overflow(uint64_t u64A, uint64_t u64B, uint64_t *pOut)
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
 * gj_lcm_n_u64 — LCM of n unsigned 64-bit integers.
 *
 * Fold pairwise: acc = lcm(acc, a[i]) with
 *   lcm(x, y) = (x / gcd(x, y)) * y
 * after reducing by the GCD so the intermediate product is minimal.
 * Overflow of that multiply, empty input, or a zero factor → 0.
 */
uint64_t
gj_lcm_n_u64(const uint64_t *pA, size_t n)
{
	size_t i;
	uint64_t u64Acc;
	uint64_t u64X;
	uint64_t u64G;
	uint64_t u64Prod;

	if (pA == NULL || n == 0u) {
		return 0u;
	}

	u64Acc = pA[0];
	for (i = 1u; i < n; i++) {
		u64X = pA[i];
		if (u64Acc == 0u || u64X == 0u) {
			return 0u;
		}
		u64G = b383_gcd(u64Acc, u64X);
		/* (acc / g) * x is exact LCM when no overflow. */
		if (b383_umul_overflow(u64Acc / u64G, u64X, &u64Prod) != 0) {
			return 0u;
		}
		u64Acc = u64Prod;
	}
	return u64Acc;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_lcm_n_u64(const uint64_t *pA, size_t n)
    __attribute__((alias("gj_lcm_n_u64")));
