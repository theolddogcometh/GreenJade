/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch301: MT19937-32 reentrant state machine.
 *
 * Surface (unique symbols; does NOT redefine rand/random/arc4random):
 *   void     gj_mt19937_seed(uint32_t *state, uint32_t seed);
 *   uint32_t gj_mt19937_next(uint32_t *state);
 *   __gj_mt19937_seed / __gj_mt19937_next  (aliases)
 *   __libcgj_batch301_marker = "libcgj-batch301"
 *
 * State layout (N+1 = 625 uint32_t words; N = 624 MT dimension):
 *   state[0..623] = mt[0..623]
 *   state[624]    = mti (0..624; 624 means twist before extract)
 *
 * Clean-room freestanding pure C. Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch301_marker[] = "libcgj-batch301";

/* MT19937-32 parameters (public specification constants). */
#define B301_N           624u
#define B301_M           397u
#define B301_MATRIX_A    0x9908b0dfu
#define B301_UPPER_MASK  0x80000000u
#define B301_LOWER_MASK  0x7fffffffu
#define B301_TEMPER_B    0x9d2c5680u
#define B301_TEMPER_C    0xefc60000u
#define B301_INIT_F      1812433253u

/* state[0..N) = mt[]; state[N] = mti */
#define B301_MT(pState, i) ((pState)[(i)])
#define B301_MTI(pState)   ((pState)[B301_N])

/* ---- internal ---------------------------------------------------------- */

/*
 * b301_twist — refill mt[0..N) via the MT19937 recurrence (one full twist).
 * Leaves mti at 0 so the next N extracts pull from the fresh vector.
 */
static void
b301_twist(uint32_t *pState)
{
	uint32_t uY;
	unsigned i;
	static const uint32_t aMag01[2] = { 0u, B301_MATRIX_A };

	for (i = 0u; i < B301_N - B301_M; i++) {
		uY = (B301_MT(pState, i) & B301_UPPER_MASK) |
		     (B301_MT(pState, i + 1u) & B301_LOWER_MASK);
		B301_MT(pState, i) =
		    B301_MT(pState, i + B301_M) ^ (uY >> 1) ^ aMag01[uY & 1u];
	}
	for (; i < B301_N - 1u; i++) {
		uY = (B301_MT(pState, i) & B301_UPPER_MASK) |
		     (B301_MT(pState, i + 1u) & B301_LOWER_MASK);
		/*
		 * i + M - N (not i + (M - N)): M and N are unsigned, so
		 * (M - N) underflows. i is in [N-M, N-2], so i+M >= N.
		 */
		B301_MT(pState, i) =
		    B301_MT(pState, i + B301_M - B301_N) ^ (uY >> 1) ^
		    aMag01[uY & 1u];
	}
	uY = (B301_MT(pState, B301_N - 1u) & B301_UPPER_MASK) |
	     (B301_MT(pState, 0u) & B301_LOWER_MASK);
	B301_MT(pState, B301_N - 1u) =
	    B301_MT(pState, B301_M - 1u) ^ (uY >> 1) ^ aMag01[uY & 1u];

	B301_MTI(pState) = 0u;
}

/*
 * b301_temper — MT19937 tempering transform (public bit-mix).
 */
static uint32_t
b301_temper(uint32_t uY)
{
	uY ^= (uY >> 11);
	uY ^= (uY << 7) & B301_TEMPER_B;
	uY ^= (uY << 15) & B301_TEMPER_C;
	uY ^= (uY >> 18);
	return uY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mt19937_seed — initialize caller-owned MT19937-32 state from seed.
 *
 * state: buffer of ≥625 uint32_t (N-word vector + mti). NULL is a no-op.
 * seed:  32-bit seed (0 is legal; init recurrence fills the vector).
 *
 * Sets mt[0] = seed, then
 *   mt[i] = 1812433253 * (mt[i-1] ^ (mt[i-1] >> 30)) + i   (mod 2^32)
 * for i = 1 .. N-1, and mti = N (twist on first next).
 */
void
gj_mt19937_seed(uint32_t *state, uint32_t seed)
{
	unsigned i;

	if (state == NULL) {
		return;
	}

	B301_MT(state, 0u) = seed;
	for (i = 1u; i < B301_N; i++) {
		uint32_t uPrev = B301_MT(state, i - 1u);

		B301_MT(state, i) =
		    B301_INIT_F * (uPrev ^ (uPrev >> 30)) + (uint32_t)i;
	}
	B301_MTI(state) = B301_N;
}

void __gj_mt19937_seed(uint32_t *state, uint32_t seed)
    __attribute__((alias("gj_mt19937_seed")));

/*
 * gj_mt19937_next — one tempered 32-bit sample; updates state in place.
 *
 * When mti >= N (including freshly seeded), performs a full twist first.
 * Returns 0 if state is NULL.
 */
uint32_t
gj_mt19937_next(uint32_t *state)
{
	uint32_t uY;
	uint32_t uMti;

	if (state == NULL) {
		return 0u;
	}

	uMti = B301_MTI(state);
	if (uMti >= B301_N) {
		b301_twist(state);
		uMti = B301_MTI(state);
	}

	uY = B301_MT(state, uMti);
	B301_MTI(state) = uMti + 1u;

	return b301_temper(uY);
}

uint32_t __gj_mt19937_next(uint32_t *state)
    __attribute__((alias("gj_mt19937_next")));
