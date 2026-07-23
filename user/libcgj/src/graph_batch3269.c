/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3269: ping-pong uint64_t inside a range (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_pingpong_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Triangle-wave fold of phase x into the closed interval [lo, hi].
 *       If lo > hi, bounds are swapped. If lo == hi, returns lo.
 *       Period is 2 * (hi - lo): rises lo→hi over the first half, then
 *       falls hi→lo over the second half. When 2*(hi-lo) would overflow
 *       uint64_t, x is treated as already inside one mathematical
 *       period (x < 2*len) without a wide modulo. No __int128.
 *   uint64_t __gj_u64_pingpong_u  (alias)
 *   __libcgj_batch3269_marker = "libcgj-batch3269"
 *
 * Milestone 3270 exclusive continuum CREATE-ONLY (3261-3270). Unique
 * gj_u64_pingpong_u surface only; no multi-def. Distinct from
 * gj_u32_pingpong_u (batch3268) / gj_u64_wrap_range_u (batch3267). No
 * parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3269_marker[] = "libcgj-batch3269";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Triangle-wave fold of phase x into [lo, hi].
 * When len > UINT64_MAX/2 the mathematical period 2*len does not fit
 * in uint64_t; every x in [0, UINT64_MAX] is then strictly less than
 * 2*len, so t = x with no modulo.
 */
static uint64_t
b3269_pingpong(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Tmp;
	uint64_t u64Len;
	uint64_t u64Period;
	uint64_t u64T;
	uint64_t u64Down;

	if (u64Lo > u64Hi) {
		u64Tmp = u64Lo;
		u64Lo = u64Hi;
		u64Hi = u64Tmp;
	}

	if (u64Lo == u64Hi) {
		return u64Lo;
	}

	u64Len = u64Hi - u64Lo;

	if (u64Len > (UINT64_MAX >> 1)) {
		/* period = 2*len does not fit; x < 2*len always. */
		u64T = u64X;
		if (u64T <= u64Len) {
			/* lo + t; t <= len so lo + t <= hi (no wrap). */
			return u64Lo + u64T;
		}
		/*
		 * Down-slope: hi - (t - len) = hi + len - t.
		 * len > 2^63-ish so hi + len may overflow uint64; use
		 * (len - (t - hi)) when hi <= t, else carefully:
		 * value = lo + (2*len - t) with 2*len as (len + (len - t))
		 * since t > len ⇒ (len - (t - len))? Wait:
		 * 2*len - t = len + (len - t) but len - t underflows.
		 * 2*len - t = len - (t - len).
		 */
		u64Down = u64Len - (u64T - u64Len);
		return u64Lo + u64Down;
	}

	u64Period = u64Len << 1;
	u64T = u64X % u64Period;

	if (u64T <= u64Len) {
		return u64Lo + u64T;
	}
	return u64Lo + (u64Period - u64T);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_pingpong_u - triangle-wave fold of x into [lo, hi].
 *
 * x:  free-running phase
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns a value in [min(lo,hi), max(lo,hi)] oscillating lo→hi→lo
 * with period 2*(hi-lo). No __int128. No parent wires.
 */
uint64_t
gj_u64_pingpong_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	(void)NULL;
	return b3269_pingpong(u64X, u64Lo, u64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_pingpong_u(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_pingpong_u")));
