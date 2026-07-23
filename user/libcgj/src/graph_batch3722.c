/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3722: freestanding xorshift64 PRNG step (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rng_xorshift64_u(uint64_t *state);
 *     - Advance caller-owned *state one Marsaglia xorshift64 step
 *       (shifts 13, 7, 17) and return the new state/sample. NULL state
 *       -> 0 (no write). *state == 0 is rewritten to 1 before stepping
 *       so the generator does not stick at the all-zero fixed point.
 *   uint64_t __gj_rng_xorshift64_u  (alias)
 *   __libcgj_batch3722_marker = "libcgj-batch3722"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_xorshift64_next (batch715, which returns 0 on zero state without
 * rewrite) — unique _u surface forces non-zero; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Not cryptographically secure. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3722_marker[] = "libcgj-batch3722";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One xorshift64 step on u64X (must be non-zero on entry).
 * Classic Marsaglia triple (13, 7, 17).
 */
static uint64_t
b3722_step(uint64_t u64X)
{
	u64X ^= u64X << 13;
	u64X ^= u64X >> 7;
	u64X ^= u64X << 17;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_xorshift64_u — one reentrant xorshift64 step.
 *
 * state: caller-owned 64-bit word (updated in place)
 *
 * Returns the new *state (also the sample), or 0 if state is NULL.
 * Zero state is forced to 1 before the step.
 */
uint64_t
gj_rng_xorshift64_u(uint64_t *state)
{
	uint64_t u64X;

	(void)NULL;

	if (state == NULL) {
		return 0ULL;
	}

	u64X = *state;
	if (u64X == 0ULL) {
		u64X = 1ULL;
	}

	u64X = b3722_step(u64X);
	*state = u64X;
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rng_xorshift64_u(uint64_t *state)
    __attribute__((alias("gj_rng_xorshift64_u")));
