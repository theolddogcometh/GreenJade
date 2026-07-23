/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4234: xorshift64 step.
 *
 * Surface (unique symbols):
 *   uint64_t gj_xorshift64_next_u2(uint64_t *state);
 *     - Advance caller-owned *state with Marsaglia xorshift64 (13,7,17)
 *       and return the new state word. NULL state -> 0 (no write).
 *       If *state == 0, seed to 1 before stepping (zero is a fixed point).
 *   uint64_t __gj_xorshift64_next_u2  (alias)
 *   __libcgj_batch4234_marker = "libcgj-batch4234"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_xorshift64_next_u2 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4234_marker[] = "libcgj-batch4234";

/* ---- freestanding helpers ---------------------------------------------- */

/* One xorshift64 triple on a non-zero word: (13, 7, 17). */
static uint64_t
b4234_step(uint64_t u64X)
{
	u64X ^= u64X << 13;
	u64X ^= u64X >> 7;
	u64X ^= u64X << 17;
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xorshift64_next_u2 - one xorshift64 step; updates *state.
 *
 * state: caller-owned 64-bit word. NULL -> 0. *state == 0 is seeded to 1
 *        before the step so the generator leaves the zero fixed point.
 *
 * Returns the new *state value, or 0 if state is NULL.
 * No parent wires.
 */
uint64_t
gj_xorshift64_next_u2(uint64_t *state)
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

	u64X = b4234_step(u64X);
	*state = u64X;
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_xorshift64_next_u2(uint64_t *state)
    __attribute__((alias("gj_xorshift64_next_u2")));
