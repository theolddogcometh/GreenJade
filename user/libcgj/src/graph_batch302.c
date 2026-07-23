/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch302: xoshiro256** PRNG (Blackman & Vigna).
 *
 * Surface (unique symbols):
 *   void gj_xoshiro256_seed(uint64_t s[4], uint64_t seed);
 *     — Seed a 256-bit xoshiro256** state from a single 64-bit seed via
 *       SplitMix64 expansion into s[0..3]. Avoids the all-zero fixed point.
 *       s may not be NULL (no-op if NULL).
 *   uint64_t gj_xoshiro256_next(uint64_t s[4]);
 *     — Advance state and return the next 64-bit output (xoshiro256**).
 *       Returns 0 if s is NULL (no errno in freestanding).
 *   __gj_xoshiro256_seed / __gj_xoshiro256_next  (aliases)
 *   __libcgj_batch302_marker = "libcgj-batch302"
 *
 * Algorithm (public specification — clean-room pure C, no source copy):
 *   result = rotl(s[1] * 5, 7) * 9
 *   t = s[1] << 17
 *   s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]
 *   s[2] ^= t; s[3] = rotl(s[3], 45)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch302_marker[] = "libcgj-batch302";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b302_rotl64 — rotate left a 64-bit word by nBits (0..63).
 */
static uint64_t
b302_rotl64(uint64_t u64X, unsigned nBits)
{
	return (u64X << nBits) | (u64X >> (64u - nBits));
}

/*
 * b302_splitmix64 — one SplitMix64 step; *pU64State is advanced in place.
 *
 * Used only to expand a single seed into four non-zero state words so
 * xoshiro256** does not land on the all-zero fixed point.
 */
static uint64_t
b302_splitmix64(uint64_t *pU64State)
{
	uint64_t u64Z;

	*pU64State += 0x9e3779b97f4a7c15ULL;
	u64Z = *pU64State;
	u64Z = (u64Z ^ (u64Z >> 30)) * 0xbf58476d1ce4e5b9ULL;
	u64Z = (u64Z ^ (u64Z >> 27)) * 0x94d049bb133111ebULL;
	return u64Z ^ (u64Z >> 31);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xoshiro256_seed — fill s[0..3] from a single 64-bit seed.
 *
 * s:    destination state array (four uint64_t words); NULL → no-op
 * seed: arbitrary 64-bit seed (all values accepted, including 0)
 *
 * Expansion uses four successive SplitMix64 outputs starting from seed.
 * That expansion is the recommended practical seeding for xoshiro256**.
 */
void
gj_xoshiro256_seed(uint64_t s[4], uint64_t seed)
{
	uint64_t u64Sm;
	unsigned i;

	if (s == NULL) {
		return;
	}

	u64Sm = seed;
	for (i = 0u; i < 4u; i++) {
		s[i] = b302_splitmix64(&u64Sm);
	}

	/*
	 * Defensive: if SplitMix somehow produced all zeros (should not),
	 * force a non-zero state so next() is not stuck.
	 */
	if ((s[0] | s[1] | s[2] | s[3]) == 0ULL) {
		s[0] = 0x9e3779b97f4a7c15ULL;
		s[1] = 0xbf58476d1ce4e5b9ULL;
		s[2] = 0x94d049bb133111ebULL;
		s[3] = 0x2545f4914f6cdd1dULL;
	}
}

void __gj_xoshiro256_seed(uint64_t s[4], uint64_t seed)
    __attribute__((alias("gj_xoshiro256_seed")));

/*
 * gj_xoshiro256_next — xoshiro256** step: scramble output + scramble state.
 *
 * s: state array of four uint64_t words (mutated in place).
 *    NULL → returns 0 without writing.
 *
 * Output scrambler is the ** variant: rotl(s1 * 5, 7) * 9.
 * Returns the 64-bit result and updates s for the next call.
 */
uint64_t
gj_xoshiro256_next(uint64_t s[4])
{
	uint64_t u64Result;
	uint64_t u64T;

	if (s == NULL) {
		return 0ULL;
	}

	u64Result = b302_rotl64(s[1] * 5ULL, 7u) * 9ULL;
	u64T = s[1] << 17;

	s[2] ^= s[0];
	s[3] ^= s[1];
	s[1] ^= s[2];
	s[0] ^= s[3];

	s[2] ^= u64T;
	s[3] = b302_rotl64(s[3], 45u);

	return u64Result;
}

uint64_t __gj_xoshiro256_next(uint64_t s[4])
    __attribute__((alias("gj_xoshiro256_next")));
