/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2267: saturating subtract of signed millisecond
 * durations (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   int64_t gj_duration_ms_sub_sat(int64_t a_ms, int64_t b_ms);
 *     - Return a_ms - b_ms for signed millisecond durations, saturating
 *       at INT64_MAX on positive overflow and INT64_MIN on negative
 *       overflow (no wrap).
 *   int64_t __gj_duration_ms_sub_sat  (alias)
 *   __libcgj_batch2267_marker = "libcgj-batch2267"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_sat_sub_u64 (batch377, unsigned floor at 0) and
 * gj_duration_ms_add_sat (batch2266). Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2267_marker[] = "libcgj-batch2267";

#define B2267_I64_MAX  0x7fffffffffffffffLL
#define B2267_I64_MIN  (-B2267_I64_MAX - 1)

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b2267_sub_sat(int64_t a, int64_t b)
{
	/*
	 * a - b overflows iff:
	 *   b > 0 && a < INT64_MIN + b
	 *   b < 0 && a > INT64_MAX + b
	 */
	if (b > (int64_t)0) {
		if (a < B2267_I64_MIN + b) {
			return B2267_I64_MIN;
		}
	} else if (b < (int64_t)0) {
		if (a > B2267_I64_MAX + b) {
			return B2267_I64_MAX;
		}
	}
	return a - b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_ms_sub_sat - saturating signed millisecond duration subtract.
 *
 * a_ms, b_ms: signed millisecond durations
 *
 * If a_ms - b_ms would overflow int64_t, returns INT64_MAX (positive
 * overflow, e.g. large positive minus large negative) or INT64_MIN
 * (negative overflow). Otherwise returns the exact difference. Pure;
 * no side effects.
 */
int64_t
gj_duration_ms_sub_sat(int64_t a_ms, int64_t b_ms)
{
	(void)NULL;
	return b2267_sub_sat(a_ms, b_ms);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_duration_ms_sub_sat(int64_t a_ms, int64_t b_ms)
    __attribute__((alias("gj_duration_ms_sub_sat")));
