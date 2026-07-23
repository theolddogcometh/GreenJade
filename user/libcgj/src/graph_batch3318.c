/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3318: deterministic Miller–Rabin on uint32_t.
 *
 * Surface (unique symbols):
 *   int gj_u32_miller_rabin_u(uint32_t n);
 *     — 1 if n is prime, 0 if composite or n < 2.
 *       Deterministic MR using the known 3-base witness set {2, 7, 61}
 *       that covers every 32-bit integer.
 *   int __gj_u32_miller_rabin_u  (alias)
 *   __libcgj_batch3318_marker = "libcgj-batch3318"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u32_miller_rabin_u surface only; no multi-def. Distinct from
 * gj_is_prime_u32 / gj_is_prime_u32_u / gj_is_prime_u64. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3318_marker[] = "libcgj-batch3318";

/*
 * Deterministic Miller–Rabin bases for every n < 2^32.
 * The set {2, 7, 61} is a standard SPRP witness set: any 32-bit odd n
 * that is a strong probable prime to all three bases is prime.
 */
static const uint32_t s_b3318_bases[3] = {
	2u,
	7u,
	61u
};

/* ---- freestanding helpers ---------------------------------------------- */

/* (a * b) mod m for m > 1; product fits in uint64. */
static uint32_t
b3318_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* (base^exp) mod m, m > 1. */
static uint32_t
b3318_powmod(uint32_t u32Base, uint32_t u32Exp, uint32_t u32M)
{
	uint32_t u32R = 1u;

	u32Base %= u32M;
	while (u32Exp != 0u) {
		if ((u32Exp & 1u) != 0u) {
			u32R = b3318_mulmod(u32R, u32Base, u32M);
		}
		u32Base = b3318_mulmod(u32Base, u32Base, u32M);
		u32Exp >>= 1;
	}
	return u32R;
}

/*
 * One strong pseudoprime test: is n an SPRP to base a?
 * Preconditions: n is odd, n > 2, 0 < a < n.
 */
static int
b3318_sprp(uint32_t u32N, uint32_t u32A)
{
	uint32_t u32D = u32N - 1u;
	unsigned uS = 0u;
	uint32_t u32X;
	unsigned uR;

	while ((u32D & 1u) == 0u) {
		u32D >>= 1;
		uS++;
	}

	u32X = b3318_powmod(u32A, u32D, u32N);
	if (u32X == 1u || u32X == u32N - 1u) {
		return 1;
	}
	for (uR = 1u; uR < uS; uR++) {
		u32X = b3318_mulmod(u32X, u32X, u32N);
		if (u32X == u32N - 1u) {
			return 1;
		}
		if (u32X == 1u) {
			return 0; /* nontrivial square root of 1 */
		}
	}
	return 0;
}

/* Deterministic Miller–Rabin for full uint32 range. */
static int
b3318_is_prime(uint32_t u32N)
{
	unsigned uI;

	if (u32N < 2u) {
		return 0;
	}
	/* Tiny primes (also factors of the hashed bases). */
	if (u32N == 2u || u32N == 3u || u32N == 5u || u32N == 7u ||
	    u32N == 11u || u32N == 13u || u32N == 17u || u32N == 19u ||
	    u32N == 23u || u32N == 29u || u32N == 31u || u32N == 37u ||
	    u32N == 41u || u32N == 43u || u32N == 47u || u32N == 53u ||
	    u32N == 61u) {
		return 1;
	}
	if ((u32N & 1u) == 0u) {
		return 0;
	}
	if (u32N % 3u == 0u || u32N % 5u == 0u || u32N % 7u == 0u ||
	    u32N % 11u == 0u || u32N % 13u == 0u || u32N % 17u == 0u ||
	    u32N % 19u == 0u || u32N % 23u == 0u || u32N % 29u == 0u ||
	    u32N % 31u == 0u || u32N % 37u == 0u || u32N % 41u == 0u ||
	    u32N % 43u == 0u || u32N % 47u == 0u || u32N % 53u == 0u) {
		return 0;
	}

	for (uI = 0u; uI < 3u; uI++) {
		uint32_t u32A = s_b3318_bases[uI] % u32N;

		if (u32A == 0u || u32A == 1u) {
			continue;
		}
		if (b3318_sprp(u32N, u32A) == 0) {
			return 0;
		}
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_miller_rabin_u — deterministic primality test for any uint32_t.
 *
 * Returns 1 if n is prime, 0 otherwise (composite or n < 2).
 * Trial-divides by small primes first, then runs Miller–Rabin against
 * the fixed 3-base witness set that covers the full 32-bit range.
 */
int
gj_u32_miller_rabin_u(uint32_t u32N)
{
	(void)NULL;
	return b3318_is_prime(u32N);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_u32_miller_rabin_u(uint32_t u32N)
    __attribute__((alias("gj_u32_miller_rabin_u")));
