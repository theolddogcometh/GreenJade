/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1446: xoshiro128** next-step (state u32[4]).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xoshiro128_next_step(uint32_t s[4]);
 *     — Advance a 4-word xoshiro128** state in place and return the
 *       next 32-bit output. s may not be NULL (returns 0 if NULL).
 *   uint32_t __gj_xoshiro128_next_step  (alias)
 *   __libcgj_batch1446_marker = "libcgj-batch1446"
 *
 * Distinct from gj_xoshiro256_next / next_step (batch302 / batch1445).
 * Unique surface only; no multi-def. Avoids splitmix/xorshift/pcg names.
 *
 * Algorithm (Blackman & Vigna xoshiro128**, public specification):
 *   result = rotl(s[1] * 5, 7) * 9
 *   t = s[1] << 9
 *   s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]
 *   s[2] ^= t; s[3] = rotl(s[3], 11)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1446_marker[] = "libcgj-batch1446";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b1446_rotl32(uint32_t u32X, unsigned uBits)
{
	return (u32X << uBits) | (u32X >> (32u - uBits));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xoshiro128_next_step — one xoshiro128** step on s[0..3].
 *
 * pS: state array of four uint32_t words (mutated in place).
 *     NULL → returns 0 without writing.
 *
 * Output scrambler is the ** variant: rotl(s1 * 5, 7) * 9.
 * Caller must ensure state is not the all-zero fixed point.
 */
uint32_t
gj_xoshiro128_next_step(uint32_t pS[4])
{
	uint32_t u32Result;
	uint32_t u32T;

	if (pS == NULL) {
		return 0u;
	}

	u32Result = b1446_rotl32(pS[1] * 5u, 7u) * 9u;
	u32T = pS[1] << 9;

	pS[2] ^= pS[0];
	pS[3] ^= pS[1];
	pS[1] ^= pS[2];
	pS[0] ^= pS[3];

	pS[2] ^= u32T;
	pS[3] = b1446_rotl32(pS[3], 11u);

	return u32Result;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xoshiro128_next_step(uint32_t pS[4])
    __attribute__((alias("gj_xoshiro128_next_step")));
