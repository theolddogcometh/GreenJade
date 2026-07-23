/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2266: saturating add of signed millisecond
 * durations (post-2260 time exclusive surface).
 *
 * Surface (unique symbols):
 *   int64_t gj_duration_ms_add_sat(int64_t a_ms, int64_t b_ms);
 *     - Return a_ms + b_ms for signed millisecond durations, saturating
 *       at INT64_MAX on positive overflow and INT64_MIN on negative
 *       overflow (no wrap).
 *   int64_t __gj_duration_ms_add_sat  (alias)
 *   __libcgj_batch2266_marker = "libcgj-batch2266"
 *
 * Post-2260 time exclusive wave (2261-2270). Distinct from
 * gj_sat_add_u64 (batch377, unsigned) and gj_duration_ms (batch698,
 * sec+nsec convert). Unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2266_marker[] = "libcgj-batch2266";

#define B2266_I64_MAX  0x7fffffffffffffffLL
#define B2266_I64_MIN  (-B2266_I64_MAX - 1)

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b2266_add_sat(int64_t a, int64_t b)
{
	if (b > (int64_t)0) {
		if (a > B2266_I64_MAX - b) {
			return B2266_I64_MAX;
		}
	} else if (b < (int64_t)0) {
		if (a < B2266_I64_MIN - b) {
			return B2266_I64_MIN;
		}
	}
	return a + b;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_duration_ms_add_sat - saturating signed millisecond duration add.
 *
 * a_ms, b_ms: signed millisecond durations
 *
 * If a_ms + b_ms would overflow int64_t, returns INT64_MAX (positive
 * overflow) or INT64_MIN (negative overflow). Otherwise returns the
 * exact sum. Pure; no side effects.
 */
int64_t
gj_duration_ms_add_sat(int64_t a_ms, int64_t b_ms)
{
	(void)NULL;
	return b2266_add_sat(a_ms, b_ms);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_duration_ms_add_sat(int64_t a_ms, int64_t b_ms)
    __attribute__((alias("gj_duration_ms_add_sat")));
