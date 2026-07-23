/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1445: xoshiro256** next-step (state u64[4]).
 *
 * Surface (unique symbols):
 *   uint64_t gj_xoshiro256_next_step(uint64_t s[4]);
 *     — Advance a 4-word xoshiro256** state in place and return the
 *       next 64-bit output. s may not be NULL (returns 0 if NULL).
 *   uint64_t __gj_xoshiro256_next_step  (alias)
 *   __libcgj_batch1445_marker = "libcgj-batch1445"
 *
 * Distinct from gj_xoshiro256_next / gj_xoshiro256_seed (batch302).
 * Unique next_step surface only; no multi-def. Avoids splitmix /
 * xorshift / pcg names on the public surface.
 *
 * Algorithm (Blackman & Vigna xoshiro256**, public specification):
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

const char __libcgj_batch1445_marker[] = "libcgj-batch1445";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1445_rotl64(uint64_t u64X, unsigned uBits)
{
	return (u64X << uBits) | (u64X >> (64u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xoshiro256_next_step — one xoshiro256** step on s[0..3].
 *
 * pS: state array of four uint64_t words (mutated in place).
 *     NULL → returns 0 without writing.
 *
 * Output scrambler is the ** variant: rotl(s1 * 5, 7) * 9.
 * Caller must ensure state is not the all-zero fixed point.
 */
uint64_t
gj_xoshiro256_next_step(uint64_t pS[4])
{
	uint64_t u64Result;
	uint64_t u64T;

	if (pS == NULL) {
		return 0ULL;
	}

	u64Result = b1445_rotl64(pS[1] * 5ULL, 7u) * 9ULL;
	u64T = pS[1] << 17;

	pS[2] ^= pS[0];
	pS[3] ^= pS[1];
	pS[1] ^= pS[2];
	pS[0] ^= pS[3];

	pS[2] ^= u64T;
	pS[3] = b1445_rotl64(pS[3], 45u);

	return u64Result;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_xoshiro256_next_step(uint64_t pS[4])
    __attribute__((alias("gj_xoshiro256_next_step")));
