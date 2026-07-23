/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3268: ping-pong uint32_t inside a range (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pingpong_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Triangle-wave fold of phase x into the closed interval [lo, hi].
 *       If lo > hi, bounds are swapped. If lo == hi, returns lo.
 *       Period is 2 * (hi - lo): rises lo→hi over the first half, then
 *       falls hi→lo over the second half.
 *   uint32_t __gj_u32_pingpong_u  (alias)
 *   __libcgj_batch3268_marker = "libcgj-batch3268"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u32_pingpong_u surface only; no multi-def. Distinct from
 * gj_u32_wrap_range_u (batch3266). No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3268_marker[] = "libcgj-batch3268";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Triangle-wave fold of phase x into [lo, hi].
 * Uses a 64-bit period so 2*(hi-lo) never wraps a uint32 intermediate.
 */
static uint32_t
b3268_pingpong(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	uint32_t u32Tmp;
	uint64_t u64Len;
	uint64_t u64Period;
	uint64_t u64T;

	if (u32Lo > u32Hi) {
		u32Tmp = u32Lo;
		u32Lo = u32Hi;
		u32Hi = u32Tmp;
	}

	if (u32Lo == u32Hi) {
		return u32Lo;
	}

	u64Len = (uint64_t)u32Hi - (uint64_t)u32Lo;
	u64Period = u64Len << 1;
	u64T = (uint64_t)u32X % u64Period;

	if (u64T <= u64Len) {
		return u32Lo + (uint32_t)u64T;
	}
	/* Down-slope: lo + (period - t) = hi - (t - len). */
	return u32Lo + (uint32_t)(u64Period - u64T);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pingpong_u - triangle-wave fold of x into [lo, hi].
 *
 * x:  free-running phase
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns a value in [min(lo,hi), max(lo,hi)] oscillating lo→hi→lo
 * with period 2*(hi-lo). No parent wires.
 */
uint32_t
gj_u32_pingpong_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b3268_pingpong(u32X, u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pingpong_u(uint32_t u32X, uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u32_pingpong_u")));
