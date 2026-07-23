/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2181: reentrant xorshift32 step (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xorshift32_next_u(uint32_t *state);
 *     - Advance caller-owned *state with Marsaglia xorshift32 (13,17,5)
 *       and return the new state word. NULL state or *state == 0 -> 0
 *       (zero is a fixed point; no write on NULL).
 *   uint32_t __gj_xorshift32_next_u  (alias)
 *   __libcgj_batch2181_marker = "libcgj-batch2181"
 *
 * Distinct from gj_xorshift32_next (batch1018 pure value step) and from
 * gj_xorshift64_next / gj_xorshift128_* - unique _u surface; no multi-def.
 *
 * Post-2180 random exclusive wave (2181-2190). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2181_marker[] = "libcgj-batch2181";

/* ---- freestanding helpers ---------------------------------------------- */

/* One xorshift32 triple on a non-zero word: (13, 17, 5). */
static uint32_t
b2181_step(uint32_t u32X)
{
	u32X ^= u32X << 13;
	u32X ^= u32X >> 17;
	u32X ^= u32X << 5;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift32_next_u - one reentrant xorshift32 step; updates *state.
 *
 * state: caller-owned 32-bit word. Must be non-zero for a non-trivial
 *        stream (all-zero is a fixed point of the map).
 * Returns the new *state value, or 0 if state is NULL or *state is 0.
 */
uint32_t
gj_xorshift32_next_u(uint32_t *pState)
{
	uint32_t u32X;

	(void)NULL;

	if (pState == NULL) {
		return 0u;
	}

	u32X = *pState;
	if (u32X == 0u) {
		return 0u;
	}

	u32X = b2181_step(u32X);
	*pState = u32X;
	return u32X;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xorshift32_next_u(uint32_t *pState)
    __attribute__((alias("gj_xorshift32_next_u")));
