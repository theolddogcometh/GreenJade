/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch898: stream mean from (sum, count).
 *
 * Distinct from gj_online_mean_* / gj_stream_mm_* — unique symbol.
 *
 * Surface (unique symbols):
 *   int64_t gj_stream_mean(int64_t sum, uint64_t count);
 *     — floor(sum / count) when count > 0; else 0. Floor toward −∞.
 *   int64_t __gj_stream_mean  (alias)
 *   __libcgj_batch898_marker = "libcgj-batch898"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch898_marker[] = "libcgj-batch898";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b898_floor_div_u — floor(a / b) for b > 0 as uint64 divisor.
 * Uses mathematical floor (toward −∞), not C truncating division.
 */
static int64_t
b898_floor_div_u(int64_t a, uint64_t b)
{
	int64_t q;
	int64_t r;
	int64_t bb;

	/* b is in (0, INT64_MAX] for a safe signed divisor path. */
	if (b > (uint64_t)INT64_MAX) {
		/*
		 * |a| <= INT64_MAX < b ⇒ |a|/b < 1, so floor is 0 if a >= 0
		 * and -1 if a < 0 (and a != 0). a==0 → 0.
		 */
		if (a >= (int64_t)0) {
			return (int64_t)0;
		}
		return (int64_t)-1;
	}

	bb = (int64_t)b;
	q = a / bb;
	r = a % bb;
	if (r != (int64_t)0 && a < (int64_t)0) {
		q -= (int64_t)1;
	}
	return q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stream_mean — integer mean of a stream given running sum and count.
 *
 * Returns floor(sum / count). count == 0 → 0 (empty stream).
 */
int64_t
gj_stream_mean(int64_t sum, uint64_t count)
{
	if (count == (uint64_t)0) {
		return (int64_t)0;
	}
	return b898_floor_div_u(sum, count);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_stream_mean(int64_t sum, uint64_t count)
    __attribute__((alias("gj_stream_mean")));
