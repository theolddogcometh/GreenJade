/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2183: reentrant xorshift128+ step (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_xorshift128_next_u(uint64_t *s0, uint64_t *s1);
 *     - Advance caller-owned two-word xorshift128+ state (s0, s1) with
 *       shift triple (23, 18, 5). Returns s0+s1 (pre-update sum style
 *       of classic xorshift128+). NULL either pointer, or both words
 *       zero, -> 0 (no write on NULL; zero pair is a fixed point).
 *   uint64_t __gj_xorshift128_next_u  (alias)
 *   __libcgj_batch2183_marker = "libcgj-batch2183"
 *
 * Distinct from gj_xorshift128_next (batch173 process-global state).
 * Unique _u reentrant export for the post-2180 random wave; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2183_marker[] = "libcgj-batch2183";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One xorshift128+ step on (s0, s1). Writes updated pair and returns
 * the classic output (old s0 + old s1). Caller guarantees non-NULL
 * and non-(0,0) state.
 */
static uint64_t
b2183_step(uint64_t *pS0, uint64_t *pS1)
{
	uint64_t u64S1;
	uint64_t u64S0;
	uint64_t u64Result;

	u64S1 = *pS0;
	u64S0 = *pS1;
	u64Result = u64S0 + u64S1;

	*pS0 = u64S0;
	u64S1 ^= u64S1 << 23;
	*pS1 = u64S1 ^ u64S0 ^ (u64S1 >> 18) ^ (u64S0 >> 5);
	return u64Result;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift128_next_u - one reentrant xorshift128+ sample.
 *
 * s0, s1: caller-owned 64-bit state words (must not both be zero).
 * Returns one 64-bit sample, or 0 if either pointer is NULL or the
 * pair is the all-zero fixed point.
 */
uint64_t
gj_xorshift128_next_u(uint64_t *pS0, uint64_t *pS1)
{
	(void)NULL;

	if (pS0 == NULL || pS1 == NULL) {
		return 0ULL;
	}
	if (*pS0 == 0ULL && *pS1 == 0ULL) {
		return 0ULL;
	}

	return b2183_step(pS0, pS1);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_xorshift128_next_u(uint64_t *pS0, uint64_t *pS1)
    __attribute__((alias("gj_xorshift128_next_u")));
