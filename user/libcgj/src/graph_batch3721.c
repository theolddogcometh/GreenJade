/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3721: freestanding xorshift32 PRNG step (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rng_xorshift32_u(uint32_t *state);
 *     - Advance caller-owned *state one Marsaglia xorshift32 step
 *       (shifts 13, 17, 5) and return the new state/sample. NULL state
 *       -> 0 (no write). *state == 0 is rewritten to 1 before stepping
 *       so the generator does not stick at the all-zero fixed point.
 *   uint32_t __gj_rng_xorshift32_u  (alias)
 *   __libcgj_batch3721_marker = "libcgj-batch3721"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730: rng_xorshift32_u,
 * rng_xorshift64_u, rng_splitmix64_u, rng_pcg32_u, rng_uniform_u32_u,
 * rng_uniform_u64_u, rng_range_u32_u, rng_range_u64_u, rng_bool_u,
 * batch_id_3730). Distinct from gj_xorshift32_next (batch1018 value
 * form) — unique _u pointer-state surface; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. Not cryptographically secure. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3721_marker[] = "libcgj-batch3721";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One xorshift32 step on u32X (must be non-zero on entry).
 * Classic Marsaglia triple (13, 17, 5).
 */
static uint32_t
b3721_step(uint32_t u32X)
{
	u32X ^= u32X << 13;
	u32X ^= u32X >> 17;
	u32X ^= u32X << 5;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_xorshift32_u — one reentrant xorshift32 step.
 *
 * state: caller-owned 32-bit word (updated in place)
 *
 * Returns the new *state (also the sample), or 0 if state is NULL.
 * Period is 2^32 - 1 for any non-zero start (zero is forced to 1).
 */
uint32_t
gj_rng_xorshift32_u(uint32_t *state)
{
	uint32_t u32X;

	(void)NULL;

	if (state == NULL) {
		return 0u;
	}

	u32X = *state;
	if (u32X == 0u) {
		u32X = 1u;
	}

	u32X = b3721_step(u32X);
	*state = u32X;
	return u32X;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rng_xorshift32_u(uint32_t *state)
    __attribute__((alias("gj_rng_xorshift32_u")));
