/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch577: binomial coefficient C(n, k) as uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_binom_u64(unsigned n, unsigned k);
 *     — C(n, k) = n! / (k! (n-k)!). Returns 0 if k > n, or if the
 *       value does not fit in uint64_t (overflow → 0). C(n, 0) = C(n, n)
 *       = 1. Multiplicative product with gcd reduction so intermediate
 *       multiplies stay exact when the final result fits.
 *   __gj_binom_u64  (alias)
 *   __libcgj_batch577_marker = "libcgj-batch577"
 *
 * Distinct from gj_binom / __gj_binom in graph_batch234.c — no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __uint128_t. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch577_marker[] = "libcgj-batch577";

/* ---- freestanding helpers ---------------------------------------------- */

/* Euclidean GCD for uint64_t (a, b >= 0). */
static uint64_t
b577_gcd(uint64_t a, uint64_t b)
{
	while (b != 0u) {
		uint64_t t = a % b;

		a = b;
		b = t;
	}
	return a;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_binom_u64 — binomial coefficient C(n, k) as uint64_t.
 *
 * Multiplicative formula (after symmetry k = min(k, n-k)):
 *   C(n, k) = product_{i=1..k} (n - k + i) / i
 * rewritten step-wise as
 *   r ← r * (n - i) / (i + 1)  for i = 0 .. k-1 starting from r = 1
 * with gcd cancellation before each multiply so that if C(n,k) fits in
 * 64 bits, intermediate products do too whenever possible. If a multiply
 * would still exceed UINT64_MAX, return 0 (overflow).
 *
 * Edge cases:
 *   k > n          → 0
 *   k == 0 || k==n → 1
 *   overflow       → 0
 */
uint64_t
gj_binom_u64(unsigned n, unsigned k)
{
	uint64_t r;
	unsigned i;

	if (k > n) {
		return 0ull;
	}
	/* C(n,k) == C(n, n-k); smaller loop. */
	if (k > n - k) {
		k = n - k;
	}
	if (k == 0u) {
		return 1ull;
	}

	r = 1ull;
	for (i = 0u; i < k; i++) {
		uint64_t num = (uint64_t)n - (uint64_t)i;
		uint64_t den = (uint64_t)i + 1ull;
		uint64_t g;

		/* Cancel factors of den against r, then against num. */
		g = b577_gcd(r, den);
		r /= g;
		den /= g;
		g = b577_gcd(num, den);
		num /= g;
		den /= g;

		/* Remaining den must divide r (binomial identity). */
		if (den != 1ull) {
			r /= den;
		}

		if (num != 0ull && r > UINT64_MAX / num) {
			return 0ull; /* would overflow uint64_t */
		}
		r *= num;
	}
	return r;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_binom_u64(unsigned n, unsigned k)
    __attribute__((alias("gj_binom_u64")));
