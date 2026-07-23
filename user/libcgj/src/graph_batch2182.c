/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2182: reentrant xorshift64 step (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_xorshift64_next_u(uint64_t *state);
 *     - Advance caller-owned *state with Marsaglia xorshift64 (13,7,17)
 *       and return the new state word. NULL state or *state == 0 -> 0
 *       (zero is a fixed point; no write on NULL).
 *   uint64_t __gj_xorshift64_next_u  (alias)
 *   __libcgj_batch2182_marker = "libcgj-batch2182"
 *
 * Distinct from gj_xorshift64_next (batch715) by symbol name only; same
 * clean-room algorithm, exclusive _u export for the post-2180 random
 * wave. Does not redefine batch715 or gj_xorshift128_* / xorshift32_*.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2182_marker[] = "libcgj-batch2182";

/* ---- freestanding helpers ---------------------------------------------- */

/* One xorshift64 triple on a non-zero word: (13, 7, 17). */
static uint64_t
b2182_step(uint64_t u64X)
{
	u64X ^= u64X << 13;
	u64X ^= u64X >> 7;
	u64X ^= u64X << 17;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift64_next_u - one reentrant xorshift64 step; updates *state.
 *
 * state: caller-owned 64-bit word. Must be non-zero for a non-trivial
 *        stream (all-zero is a fixed point of the map).
 * Returns the new *state value, or 0 if state is NULL or *state is 0.
 */
uint64_t
gj_xorshift64_next_u(uint64_t *pState)
{
	uint64_t u64X;

	(void)NULL;

	if (pState == NULL) {
		return 0ULL;
	}

	u64X = *pState;
	if (u64X == 0ULL) {
		return 0ULL;
	}

	u64X = b2182_step(u64X);
	*pState = u64X;
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_xorshift64_next_u(uint64_t *pState)
    __attribute__((alias("gj_xorshift64_next_u")));
