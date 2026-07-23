/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch575: deterministic primality test for uint64_t.
 *
 * Surface (unique symbols):
 *   int gj_prime_p_u64(uint64_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *       Trial-divides small primes, then deterministic Miller–Rabin
 *       with the 7-base witness set that covers every 64-bit integer.
 *   __gj_prime_p_u64  (alias)
 *   __libcgj_batch575_marker = "libcgj-batch575"
 *
 * Note: the same public names also live in graph_batch231.c when that
 * TU is linked; this file is the batch575 exclusive surface. Static
 * helpers are b575_* only (no multi-def of is_prime helpers).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no __uint128_t. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch575_marker[] = "libcgj-batch575";

/*
 * Deterministic Miller–Rabin bases for every n < 2^64.
 * Any odd 64-bit n that is a strong probable prime to all seven bases
 * is prime.
 */
static const uint64_t s_b575_bases[7] = {
	2ull,
	325ull,
	9375ull,
	28178ull,
	450775ull,
	9780504ull,
	1795265022ull
};

/* ---- freestanding modular helpers (no __uint128_t) --------------------- */

/* (a * b) mod m, m > 0. Binary modular multiply — 64-bit ops only. */
static uint64_t
b575_mulmod(uint64_t a, uint64_t b, uint64_t m)
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
b575_powmod(uint64_t base, uint64_t exp, uint64_t m)
{
	uint64_t r = 1u;

	base %= m;
	while (exp != 0u) {
		if ((exp & 1u) != 0u) {
			r = b575_mulmod(r, base, m);
		}
		base = b575_mulmod(base, base, m);
		exp >>= 1;
	}
	return r;
}

/*
 * Strong pseudoprime test: is n an SPRP to base a?
 * Preconditions: n odd, n > 2, 0 < a < n.
 */
static int
b575_sprp(uint64_t n, uint64_t a)
{
	uint64_t d = n - 1u;
	unsigned s = 0u;
	uint64_t x;
	unsigned r;

	while ((d & 1u) == 0u) {
		d >>= 1;
		s++;
	}

	x = b575_powmod(a, d, n);
	if (x == 1u || x == n - 1u) {
		return 1;
	}
	for (r = 1u; r < s; r++) {
		x = b575_mulmod(x, x, n);
		if (x == n - 1u) {
			return 1;
		}
		if (x == 1u) {
			return 0;
		}
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_prime_p_u64 — deterministic primality test for any uint64_t.
 *
 * Returns 1 if n is prime, 0 otherwise (composite or n < 2).
 */
int
gj_prime_p_u64(uint64_t n)
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
		uint64_t a = s_b575_bases[i] % n;

		if (a == 0u || a == 1u) {
			continue;
		}
		if (!b575_sprp(n, a)) {
			return 0;
		}
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_prime_p_u64(uint64_t n)
    __attribute__((alias("gj_prime_p_u64")));
