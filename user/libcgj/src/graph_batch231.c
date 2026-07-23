/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch231: deterministic Miller–Rabin primality test
 * for the full uint64_t range.
 *
 * Surface (unique symbols):
 *   int gj_is_prime_u64(uint64_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *       Deterministic MR using the known 7-base witness set that covers
 *       every 64-bit integer (see comments near the base table).
 *   __gj_is_prime_u64  (alias)
 *   __libcgj_batch231_marker = "libcgj-batch231"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 * Modular arithmetic avoids __uint128_t / libgcc __umodti3 (binary mulmod).
 */

#include <stdint.h>

const char __libcgj_batch231_marker[] = "libcgj-batch231";

/*
 * Deterministic Miller–Rabin bases for every n < 2^64.
 *
 * The 7-element set
 *   {2, 325, 9375, 28178, 450775, 9780504, 1795265022}
 * is a standard SPRP witness set: any 64-bit odd n that is a strong
 * probable prime to all seven bases is prime. (Small primes that divide a
 * base are handled before the MR loop, so reducing a base mod n never
 * yields a false composite for those primes.)
 */
static const uint64_t s_b231_bases[7] = {
	2ull,
	325ull,
	9375ull,
	28178ull,
	450775ull,
	9780504ull,
	1795265022ull
};

/* ---- freestanding modular helpers (no __uint128_t) --------------------- */

/*
 * (a * b) mod m, with m > 0.
 * Binary modular multiply: only 64-bit ops, freestanding-safe.
 */
static uint64_t
b231_mulmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t r = 0u;

	a %= m;
	b %= m;
	while (b != 0u) {
		if ((b & 1u) != 0u) {
			uint64_t s = m - r;
			r = (a >= s) ? (a - s) : (r + a);
		}
		{
			uint64_t s = m - a;
			a = (a >= s) ? (a - s) : (a + a);
		}
		b >>= 1;
	}
	return r;
}

/* (base^exp) mod m, m > 1. */
static uint64_t
b231_powmod(uint64_t base, uint64_t exp, uint64_t m)
{
	uint64_t r = 1u;

	base %= m;
	while (exp != 0u) {
		if ((exp & 1u) != 0u) {
			r = b231_mulmod(r, base, m);
		}
		base = b231_mulmod(base, base, m);
		exp >>= 1;
	}
	return r;
}

/*
 * One strong pseudoprime test: is n an SPRP to base a?
 * Preconditions: n is odd, n > 2, 0 < a < n.
 * Writes n-1 = 2^s * d with d odd, then checks
 *   a^d ≡ 1  or  a^(2^r d) ≡ -1 for some r in 0..s-1  (mod n).
 */
static int
b231_sprp(uint64_t n, uint64_t a)
{
	uint64_t d = n - 1u;
	unsigned s = 0u;
	uint64_t x;
	unsigned r;

	while ((d & 1u) == 0u) {
		d >>= 1;
		s++;
	}

	x = b231_powmod(a, d, n);
	if (x == 1u || x == n - 1u) {
		return 1;
	}
	for (r = 1u; r < s; r++) {
		x = b231_mulmod(x, x, n);
		if (x == n - 1u) {
			return 1;
		}
		if (x == 1u) {
			return 0; /* nontrivial square root of 1 */
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_is_prime_u64 — deterministic primality test for any uint64_t.
 *
 * Returns 1 if n is prime, 0 otherwise (composite or n < 2).
 * Trial-divides by small primes first, then runs Miller–Rabin against
 * the fixed 7-base witness set that covers the full 64-bit range.
 */
int
gj_is_prime_u64(uint64_t n)
{
	unsigned i;

	if (n < 2u) {
		return 0;
	}
	/* Tiny primes (also factors of the hashed bases). */
	if (n == 2u || n == 3u || n == 5u || n == 7u ||
	    n == 11u || n == 13u || n == 17u || n == 19u ||
	    n == 23u || n == 29u || n == 31u || n == 37u ||
	    n == 41u || n == 43u || n == 47u || n == 53u) {
		return 1;
	}
	if ((n & 1u) == 0u) {
		return 0;
	}
	if (n % 3u == 0u || n % 5u == 0u || n % 7u == 0u ||
	    n % 11u == 0u || n % 13u == 0u || n % 17u == 0u ||
	    n % 19u == 0u || n % 23u == 0u || n % 29u == 0u ||
	    n % 31u == 0u || n % 37u == 0u || n % 41u == 0u ||
	    n % 43u == 0u || n % 47u == 0u || n % 53u == 0u) {
		return 0;
	}

	for (i = 0u; i < 7u; i++) {
		uint64_t a = s_b231_bases[i] % n;

		/* a == 0 ⇒ n divides the base; n already cleared of small
		 * prime factors of those bases, so this is a no-op skip. */
		if (a == 0u || a == 1u) {
			continue;
		}
		if (!b231_sprp(n, a)) {
			return 0;
		}
	}
	return 1;
}

int __gj_is_prime_u64(uint64_t n)
    __attribute__((alias("gj_is_prime_u64")));
