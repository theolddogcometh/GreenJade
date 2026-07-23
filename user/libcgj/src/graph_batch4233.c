/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4233: xorshift32 step (seed 0 -> 1).
 *
 * Surface (unique symbols):
 *   uint32_t gj_xorshift32_next_u2(uint32_t *state);
 *     - Advance caller-owned *state with Marsaglia xorshift32 (13,17,5)
 *       and return the new state word. NULL state -> 0 (no write).
 *       If *state == 0, seed to 1 before stepping (zero is a fixed point
 *       of the map; auto-seed keeps the stream non-trivial).
 *   uint32_t __gj_xorshift32_next_u2  (alias)
 *   __libcgj_batch4233_marker = "libcgj-batch4233"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_xorshift32_next_u2 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4233_marker[] = "libcgj-batch4233";

/* ---- freestanding helpers ---------------------------------------------- */

/* One xorshift32 triple on a non-zero word: (13, 17, 5). */
static uint32_t
b4233_step(uint32_t u32X)
{
	u32X ^= u32X << 13;
	u32X ^= u32X >> 17;
	u32X ^= u32X << 5;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift32_next_u2 - one xorshift32 step; updates *state.
 *
 * state: caller-owned 32-bit word. NULL -> 0. *state == 0 is seeded to 1
 *        before the step so the generator leaves the zero fixed point.
 *
 * Returns the new *state value, or 0 if state is NULL.
 * No parent wires.
 */
uint32_t
gj_xorshift32_next_u2(uint32_t *state)
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

	u32X = b4233_step(u32X);
	*state = u32X;
	return u32X;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_xorshift32_next_u2(uint32_t *state)
    __attribute__((alias("gj_xorshift32_next_u2")));
