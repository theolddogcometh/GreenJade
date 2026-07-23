/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4072: uint64_t triangle wave (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_tri_u(uint64_t phase, uint64_t period);
 *     - Triangle wave of the given period. Maps phase into a rising
 *       then falling ramp over one period. Returns a value in
 *       [0, period/2]. period == 0 → 0 (soft guard).
 *   uint64_t __gj_u64_tri_u  (alias)
 *   __libcgj_batch4072_marker = "libcgj-batch4072"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u64_tri_u surface only; no multi-def. Distinct from gj_u32_tri_u
 * (batch4071). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4072_marker[] = "libcgj-batch4072";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Triangle wave: rise 0..half over first half-period, fall half..0 over
 * the second. Soft-guard period == 0 → 0.
 */
static uint64_t
b4072_tri(uint64_t u64Phase, uint64_t u64Period)
{
	uint64_t u64T;
	uint64_t u64Half;

	if (u64Period == 0ull) {
		return 0ull;
	}
	u64T = u64Phase % u64Period;
	u64Half = u64Period / 2ull;
	if (u64Half == 0ull) {
		return 0ull;
	}
	if (u64T <= u64Half) {
		return u64T;
	}
	return u64Period - u64T;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_tri_u - unsigned 64-bit triangle wave.
 *
 * phase:  free-running phase sample
 * period: wave period in samples (0 → 0 soft guard)
 *
 * Returns a value in [0, period/2] forming a triangle over one period.
 * No parent wires.
 */
uint64_t
gj_u64_tri_u(uint64_t u64Phase, uint64_t u64Period)
{
	(void)NULL;
	return b4072_tri(u64Phase, u64Period);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_tri_u(uint64_t u64Phase, uint64_t u64Period)
    __attribute__((alias("gj_u64_tri_u")));
