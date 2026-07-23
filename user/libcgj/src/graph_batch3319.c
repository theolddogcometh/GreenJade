/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3319: seeded random prime on uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_random_prime_u(uint32_t *seed);
 *     — Draw a deterministic pseudo-random odd candidate from *seed
 *       (xorshift32 state), advance the seed, and return the smallest
 *       prime >= that candidate. If the search would wrap past the last
 *       32-bit prime, wraps and continues (always terminates; returns a
 *       prime in [2, 4294967291]). NULL seed → 2 (soft guard).
 *   uint32_t __gj_u32_random_prime_u  (alias)
 *   __libcgj_batch3319_marker = "libcgj-batch3319"
 *
 * CREATE-ONLY exclusive continuum wave (3311–3320). Unique
 * gj_u32_random_prime_u surface only; no multi-def. Local MR primality
 * (no parent wires to batch3318). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3319_marker[] = "libcgj-batch3319";

/* Largest prime in uint32_t: 2^32 - 5. */
static const uint32_t s_b3319_last_u32_prime = 4294967291u;

/* Deterministic MR bases for n < 2^32. */
static const uint32_t s_b3319_bases[3] = {
	2u,
	7u,
	61u
};

/* ---- freestanding helpers ---------------------------------------------- */

/* xorshift32 step; never returns 0 (maps 0 → 0xdeadbeef first). */
static uint32_t
b3319_xorshift32(uint32_t u32State)
{
	if (u32State == 0u) {
		u32State = 0xdeadbeefu;
	}
	u32State ^= u32State << 13;
	u32State ^= u32State >> 17;
	u32State ^= u32State << 5;
	return u32State;
}

/* (a * b) mod m for m > 1. */
static uint32_t
b3319_mulmod(uint32_t u32A, uint32_t u32B, uint32_t u32M)
{
	return (uint32_t)(((uint64_t)u32A * (uint64_t)u32B) % (uint64_t)u32M);
}

/* (base^exp) mod m, m > 1. */
static uint32_t
b3319_powmod(uint32_t u32Base, uint32_t u32Exp, uint32_t u32M)
{
	uint32_t u32R = 1u;

	u32Base %= u32M;
	while (u32Exp != 0u) {
		if ((u32Exp & 1u) != 0u) {
			u32R = b3319_mulmod(u32R, u32Base, u32M);
		}
		u32Base = b3319_mulmod(u32Base, u32Base, u32M);
		u32Exp >>= 1;
	}
	return u32R;
}

/* Strong pseudoprime test; n odd, n > 2, 0 < a < n. */
static int
b3319_sprp(uint32_t u32N, uint32_t u32A)
{
	uint32_t u32D = u32N - 1u;
	unsigned uS = 0u;
	uint32_t u32X;
	unsigned uR;

	while ((u32D & 1u) == 0u) {
		u32D >>= 1;
		uS++;
	}

	u32X = b3319_powmod(u32A, u32D, u32N);
	if (u32X == 1u || u32X == u32N - 1u) {
		return 1;
	}
	for (uR = 1u; uR < uS; uR++) {
		u32X = b3319_mulmod(u32X, u32X, u32N);
		if (u32X == u32N - 1u) {
			return 1;
		}
		if (u32X == 1u) {
			return 0;
		}
	}
	return 0;
}

/* Local deterministic MR primality (self-contained; no parent wire). */
static int
b3319_is_prime(uint32_t u32N)
{
	unsigned uI;

	if (u32N < 2u) {
		return 0;
	}
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
		uint32_t u32A = s_b3319_bases[uI] % u32N;

		if (u32A == 0u || u32A == 1u) {
			continue;
		}
		if (b3319_sprp(u32N, u32A) == 0) {
			return 0;
		}
	}
	return 1;
}

/*
 * Smallest prime >= u32C. If none exists below 2^32, wrap to 2 and
 * continue (there is always a prime).
 */
static uint32_t
b3319_next_prime_ge(uint32_t u32C)
{
	uint32_t u32Start;

	if (u32C <= 2u) {
		return 2u;
	}
	if (u32C > s_b3319_last_u32_prime) {
		return 2u;
	}
	if ((u32C & 1u) == 0u) {
		u32C++;
	}

	u32Start = u32C;
	for (;;) {
		if (b3319_is_prime(u32C)) {
			return u32C;
		}
		if (u32C >= s_b3319_last_u32_prime) {
			/* wrap once; guaranteed to hit a prime before u32Start */
			u32C = 3u;
		} else {
			u32C += 2u;
		}
		if (u32C == u32Start) {
			/* should never happen; defensive */
			return 2u;
		}
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_random_prime_u — seeded pseudo-random prime in uint32_t.
 *
 * seed: in/out xorshift32 state (NULL → soft return 2, no update)
 *
 * Advances *seed once via xorshift32, then returns the smallest prime
 * greater than or equal to the drawn candidate (odd-forced). Deterministic
 * for a given seed sequence. No libc rand().
 */
uint32_t
gj_u32_random_prime_u(uint32_t *pSeed)
{
	uint32_t u32S;
	uint32_t u32Cand;

	(void)NULL;

	if (pSeed == NULL) {
		return 2u;
	}

	u32S = b3319_xorshift32(*pSeed);
	*pSeed = u32S;

	/* Force odd candidate in [3, UINT32_MAX]; keep 0/1 → 3. */
	u32Cand = u32S | 1u;
	if (u32Cand < 3u) {
		u32Cand = 3u;
	}

	return b3319_next_prime_ge(u32Cand);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_random_prime_u(uint32_t *pSeed)
    __attribute__((alias("gj_u32_random_prime_u")));
