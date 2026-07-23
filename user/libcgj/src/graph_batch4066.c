/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4066: rectangular pulse on uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pulse_u(uint32_t edge0, uint32_t edge1, uint32_t x);
 *     - Rectangular pulse: step(edge0, x) - step(edge1, x) semantics
 *       on the unsigned domain. When edge0 < edge1, returns 1 iff
 *       edge0 <= x < edge1. When edge0 > edge1, returns 1 iff
 *       edge1 <= x < edge0. When edge0 == edge1, returns 0.
 *   uint32_t __gj_u32_pulse_u  (alias)
 *   __libcgj_batch4066_marker = "libcgj-batch4066"
 *
 * Exclusive continuum CREATE-ONLY (4061-4070). Unique gj_u32_pulse_u
 * surface only; no multi-def. Distinct from gj_u32_in_range_u
 * (batch3264 closed membership) and gj_u32_step_u (batch4064).
 * No parent wires (step inlined). No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4066_marker[] = "libcgj-batch4066";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Half-open pulse on the ordered span [lo, hi): 1 when lo <= x < hi.
 * Empty span (lo == hi) yields 0.
 */
static uint32_t
b4066_pulse(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	uint32_t u32Lo;
	uint32_t u32Hi;

	if (u32Edge0 == u32Edge1) {
		return 0u;
	}

	if (u32Edge0 < u32Edge1) {
		u32Lo = u32Edge0;
		u32Hi = u32Edge1;
	} else {
		u32Lo = u32Edge1;
		u32Hi = u32Edge0;
	}

	if (u32X >= u32Lo && u32X < u32Hi) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pulse_u - rectangular half-open pulse on uint32_t.
 *
 * edge0, edge1: pulse endpoints (order independent; empty → 0)
 * x:            sample value
 *
 * Returns 1 when x lies in the half-open interval between the ordered
 * endpoints, else 0. Equivalent to |step(edge0,x) - step(edge1,x)| for
 * unsigned edges. No parent wires.
 */
uint32_t
gj_u32_pulse_u(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
{
	(void)NULL;
	return b4066_pulse(u32Edge0, u32Edge1, u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pulse_u(uint32_t u32Edge0, uint32_t u32Edge1, uint32_t u32X)
    __attribute__((alias("gj_u32_pulse_u")));
