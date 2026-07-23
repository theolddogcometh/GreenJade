/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch382: n-ary GCD of uint64_t values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_gcd_n_u64(const uint64_t *a, size_t n);
 *     — Greatest common divisor of a[0..n). Uses a local Euclidean
 *       helper (does not call or redefine gj_gcd_u64). Conventions:
 *         n == 0 or a == NULL with n > 0 → 0
 *         gcd of a single element → that element
 *         zeros are identity under binary gcd (gcd(0, x) == x)
 *         empty / all-zero inputs → 0
 *   uint64_t __gj_gcd_n_u64  (alias)
 *   __libcgj_batch382_marker = "libcgj-batch382"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Does not redefine gj_gcd_u64
 * (owned by graph_batch229.c).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch382_marker[] = "libcgj-batch382";

/* ---- helpers ----------------------------------------------------------- */

/*
 * b382_gcd — Euclidean GCD on two uint64_t values.
 *
 * Local static only; must not collide with public gj_gcd_u64.
 * gcd(0, 0) → 0; gcd(x, 0) / gcd(0, x) → x.
 */
static uint64_t
b382_gcd(uint64_t u64A, uint64_t u64B)
{
	while (u64B != 0u) {
		uint64_t u64T = u64A % u64B;

		u64A = u64B;
		u64B = u64T;
	}
	return u64A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gcd_n_u64 — GCD of n unsigned 64-bit integers.
 *
 * a: pointer to n values (read-only); NULL only legal when n == 0
 * n: element count
 *
 * Fold left with binary GCD. Early-out when running GCD reaches 1
 * (further elements cannot change the result).
 */
uint64_t
gj_gcd_n_u64(const uint64_t *a, size_t n)
{
	uint64_t u64G;
	size_t i;

	if (n == 0u || a == NULL) {
		return 0u;
	}

	u64G = a[0];
	for (i = 1u; i < n; i++) {
		if (u64G == 1u) {
			return 1u;
		}
		u64G = b382_gcd(u64G, a[i]);
	}
	return u64G;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_gcd_n_u64(const uint64_t *a, size_t n)
    __attribute__((alias("gj_gcd_n_u64")));
