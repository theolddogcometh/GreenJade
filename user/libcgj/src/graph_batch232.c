/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch232: Pollard's rho integer factorization.
 *
 * Surface (unique symbols):
 *   uint64_t gj_pollard_rho(uint64_t n);
 *     — Return one non-trivial factor of n, or n itself when n < 2,
 *       n is prime, or the search does not produce a proper factor.
 *       Even n → 2. Deterministic seeds / polynomials (no RNG).
 *   __gj_pollard_rho  (alias)
 *   __libcgj_batch232_marker = "libcgj-batch232"
 *
 * Algorithm (Floyd cycle detection, Brent-style product batching of
 * |x-y| differences before gcd):
 *   f_c(x) = x^2 + c  (mod n)
 *   Tortoise advances 1 step, hare 2; periodically take gcd of the
 *   product of successive |x-y| with n. On trivial factor n, retry
 *   with a new constant c and starting point.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2. No malloc, no errno. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch232_marker[] = "libcgj-batch232";

/* Max outer (c, seed) attempts before giving up and returning n. */
#define B232_MAX_C 32u
/* Steps between gcd evaluations (product batch size). */
#define B232_BATCH 128u
/* Hard cap on tortoise advances per (c, seed) pair. */
#define B232_MAX_ITERS (1u << 20)

/* ---- freestanding helpers ---------------------------------------------- */

/* Binary GCD; gcd(0,0) → 0. */
static uint64_t
b232_gcd(uint64_t a, uint64_t b)
{
	unsigned shift;

	if (a == 0u) {
		return b;
	}
	if (b == 0u) {
		return a;
	}

	shift = 0u;
	while (((a | b) & 1u) == 0u) {
		a >>= 1;
		b >>= 1;
		shift++;
	}
	while ((a & 1u) == 0u) {
		a >>= 1;
	}
	do {
		while ((b & 1u) == 0u) {
			b >>= 1;
		}
		if (a > b) {
			uint64_t t = a;
			a = b;
			b = t;
		}
		b -= a;
	} while (b != 0u);

	return a << shift;
}

/* (a + b) mod m, with 0 <= a,b < m (so a+b < 2m). */
static uint64_t
b232_addmod(uint64_t a, uint64_t b, uint64_t m)
{
	/* a >= m-b  ⇔  a+b >= m; result a-(m-b) avoids uint64 wrap. */
	if (a >= m - b) {
		return a - (m - b);
	}
	return a + b;
}

/*
 * (a * b) mod m for m > 0 — pure uint64, no libgcc 128-bit helpers.
 * Russian-peasant multiply with modular doubling/adding.
 */
static uint64_t
b232_mulmod(uint64_t a, uint64_t b, uint64_t m)
{
	uint64_t r = 0u;

	if (m == 0u) {
		return 0u;
	}
	a %= m;
	b %= m;
	while (b != 0u) {
		if ((b & 1u) != 0u) {
			r = b232_addmod(r, a, m);
		}
		a = b232_addmod(a, a, m);
		b >>= 1;
	}
	return r;
}

/* (x * x + c) mod n, with n > 0. */
static uint64_t
b232_f(uint64_t x, uint64_t c, uint64_t n)
{
	uint64_t sq = b232_mulmod(x, x, n);
	uint64_t cc = c % n;

	return b232_addmod(sq, cc, n);
}

/* |a - b| mod n  (for a,b in [0,n)). Equivalent to abs diff when a,b < n. */
static uint64_t
b232_absdiff(uint64_t a, uint64_t b)
{
	return (a > b) ? (a - b) : (b - a);
}

/*
 * One Pollard-rho attempt with polynomial x^2+c and start x0.
 * Returns a proper factor in (1, n), or 0 if none found this try.
 */
static uint64_t
b232_rho_once(uint64_t n, uint64_t c, uint64_t x0)
{
	uint64_t x = x0 % n;
	uint64_t y = x;
	uint64_t d = 1u;
	uint64_t prod = 1u;
	uint32_t steps = 0u;
	uint32_t batch = 0u;

	if (c >= n) {
		c %= n;
	}
	if (c == 0u) {
		c = 1u;
	}

	while (d == 1u && steps < B232_MAX_ITERS) {
		x = b232_f(x, c, n);
		y = b232_f(y, c, n);
		y = b232_f(y, c, n);

		{
			uint64_t diff = b232_absdiff(x, y);

			if (diff != 0u) {
				prod = b232_mulmod(prod, diff, n);
			}
		}

		batch++;
		steps++;
		if (batch >= B232_BATCH || x == y) {
			d = b232_gcd(prod, n);
			prod = 1u;
			batch = 0u;
			if (d == n) {
				/* Backtrack style: recompute with single gcds from
				 * a saved tortoise/hare is heavy; restart attempt. */
				return 0u;
			}
		}
	}

	if (d > 1u && d < n) {
		return d;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pollard_rho — one non-trivial factor of n, or n.
 *
 *   n < 2          → n
 *   n even         → 2
 *   composite      → a factor in (1, n) when found
 *   prime / fail   → n
 *
 * Deterministic: tries a fixed sequence of (c, x0) pairs. Does not prove
 * primality; a return of n only means no factor was found by rho.
 */
uint64_t
gj_pollard_rho(uint64_t n)
{
	uint32_t i;
	/* Fixed (c, x0) schedule — no RNG, freestanding-safe. */
	static const uint64_t c_tab[8] = {
	    1u, 2u, 3u, 5u, 7u, 11u, 13u, 17u
	};
	static const uint64_t x_tab[4] = {
	    2u, 3u, 5u, 7u
	};

	if (n < 2u) {
		return n;
	}
	if ((n & 1u) == 0u) {
		return 2u;
	}
	/* Quick reject for small primes (optional speed; still correct without). */
	if (n % 3u == 0u) {
		return (n == 3u) ? n : 3u;
	}
	if (n % 5u == 0u) {
		return (n == 5u) ? n : 5u;
	}

	for (i = 0u; i < B232_MAX_C; i++) {
		uint64_t c = c_tab[i & 7u] + (uint64_t)(i >> 3);
		uint64_t x0 = x_tab[i & 3u] + (uint64_t)i;
		uint64_t f = b232_rho_once(n, c, x0);

		if (f > 1u && f < n) {
			return f;
		}
	}

	return n;
}

uint64_t __gj_pollard_rho(uint64_t n)
    __attribute__((alias("gj_pollard_rho")));
