/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4067: rectangular pulse on uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pulse_u(uint64_t edge0, uint64_t edge1, uint64_t x);
 *     - Rectangular pulse: step(edge0, x) - step(edge1, x) semantics
 *       on the unsigned domain. When edge0 < edge1, returns 1 iff
 *       edge0 <= x < edge1. When edge0 > edge1, returns 1 iff
 *       edge1 <= x < edge0. When edge0 == edge1, returns 0.
 *   uint64_t __gj_u64_pulse_u  (alias)
 *   __libcgj_batch4067_marker = "libcgj-batch4067"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u64_pulse_u
 * surface only; no multi-def. Distinct from gj_u32_pulse_u (batch4066),
 * gj_u64_in_range_u (batch3265), and gj_u64_step_u (batch4065).
 * No parent wires (step inlined). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4067_marker[] = "libcgj-batch4067";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Half-open pulse on the ordered span [lo, hi): 1 when lo <= x < hi.
 * Empty span (lo == hi) yields 0.
 */
static uint64_t
b4067_pulse(uint64_t u64Edge0, uint64_t u64Edge1, uint64_t u64X)
{
	uint64_t u64Lo;
	uint64_t u64Hi;

	if (u64Edge0 == u64Edge1) {
		return 0ull;
	}

	if (u64Edge0 < u64Edge1) {
		u64Lo = u64Edge0;
		u64Hi = u64Edge1;
	} else {
		u64Lo = u64Edge1;
		u64Hi = u64Edge0;
	}

	if (u64X >= u64Lo && u64X < u64Hi) {
		return 1ull;
	}
	return 0ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pulse_u - rectangular half-open pulse on uint64_t.
 *
 * edge0, edge1: pulse endpoints (order independent; empty → 0)
 * x:            sample value
 *
 * Returns 1 when x lies in the half-open interval between the ordered
 * endpoints, else 0. No parent wires.
 */
uint64_t
gj_u64_pulse_u(uint64_t u64Edge0, uint64_t u64Edge1, uint64_t u64X)
{
	(void)NULL;
	return b4067_pulse(u64Edge0, u64Edge1, u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pulse_u(uint64_t u64Edge0, uint64_t u64Edge1, uint64_t u64X)
    __attribute__((alias("gj_u64_pulse_u")));
