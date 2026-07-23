/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4235: inclusive u32 range via xorshift32.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_rand_range_u(uint32_t *state, uint32_t lo, uint32_t hi);
 *     - Inclusive uniform-ish sample in [lo, hi] using Marsaglia
 *       xorshift32 on caller-owned *state. If lo > hi, swap. If lo == hi,
 *       return lo without advancing. NULL state -> lo (after swap).
 *       Zero *state is seeded to 1 before stepping.
 *   uint32_t __gj_u32_rand_range_u  (alias)
 *   __libcgj_batch4235_marker = "libcgj-batch4235"
 *
 * Exclusive continuum CREATE-ONLY (4231-4240: splitmix64_next_u,
 * splitmix64_u32_u, xorshift32_next_u, xorshift64_next_u,
 * u32_rand_range_u, u64_mix_const_u, u32_pcg_step_u, u32_from_u64_mix_u,
 * seed_from_ptr_u, batch_id_4240). Unique gj_u32_rand_range_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. Not cryptographically secure.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4235_marker[] = "libcgj-batch4235";

/* ---- freestanding helpers ---------------------------------------------- */

/* One xorshift32 triple: (13, 17, 5). */
static uint32_t
b4235_xorshift32(uint32_t u32X)
{
	u32X ^= u32X << 13;
	u32X ^= u32X >> 17;
	u32X ^= u32X << 5;
	return u32X;
}

/* Advance *pState (auto-seed 0->1); return new state word. */
static uint32_t
b4235_next(uint32_t *pState)
{
	uint32_t u32X;

	u32X = *pState;
	if (u32X == 0u) {
		u32X = 1u;
	}
	u32X = b4235_xorshift32(u32X);
	*pState = u32X;
	return u32X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_rand_range_u - inclusive random u32 in [lo, hi] via xorshift32.
 *
 * state: caller-owned xorshift32 state; advanced once when lo != hi.
 *        NULL -> returns lo after any swap (no write).
 * lo/hi: inclusive bounds; swapped if lo > hi; equal -> return lo.
 *
 * Full-range [0, UINT32_MAX] returns the raw xorshift word. Otherwise
 * maps via multiply-high (Lemire-style) of the 32-bit sample against
 * (span) to reduce low-bit bias. No parent wires.
 */
uint32_t
gj_u32_rand_range_u(uint32_t *state, uint32_t lo, uint32_t hi)
{
	uint32_t u32Tmp;
	uint32_t u32Span;
	uint32_t u32R;
	uint64_t u64Prod;

	(void)NULL;

	if (lo > hi) {
		u32Tmp = lo;
		lo = hi;
		hi = u32Tmp;
	}

	if (lo == hi) {
		return lo;
	}

	if (state == NULL) {
		return lo;
	}

	/* span = hi - lo + 1; full range wraps to 0. */
	u32Span = hi - lo + 1u;
	u32R = b4235_next(state);

	if (u32Span == 0u) {
		/* [0, UINT32_MAX] — full 32-bit domain. */
		return u32R;
	}

	u64Prod = (uint64_t)u32R * (uint64_t)u32Span;
	return lo + (uint32_t)(u64Prod >> 32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_rand_range_u(uint32_t *state, uint32_t lo, uint32_t hi)
    __attribute__((alias("gj_u32_rand_range_u")));
