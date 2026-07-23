/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch590: median of three uint64_t values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_median3_u64(uint64_t a, uint64_t b, uint64_t c);
 *     — Return the median (middle value) of a, b, and c under
 *       unsigned 64-bit order. Equal values are fine; any of the
 *       three may be returned when ties make more than one median
 *       candidate identical. Self-contained branch form (no helpers).
 *   __gj_median3_u64  (alias)
 *   __libcgj_batch590_marker = "libcgj-batch590"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch590_marker[] = "libcgj-batch590";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_median3_u64 — median of three uint64_t values.
 *
 * a, b, c: candidates
 *
 * Returns the middle value when the three are ordered. Implemented as
 * pairwise "is x between y and z" tests so the TU stays branch-only
 * and free of nested min/max helpers.
 */
uint64_t
gj_median3_u64(uint64_t a, uint64_t b, uint64_t c)
{
	/* a is median when it sits between b and c (inclusive). */
	if ((b <= a && a <= c) || (c <= a && a <= b)) {
		return a;
	}
	/* b is median when it sits between a and c (inclusive). */
	if ((a <= b && b <= c) || (c <= b && b <= a)) {
		return b;
	}
	/* Otherwise c is the remaining middle value. */
	return c;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_median3_u64(uint64_t a, uint64_t b, uint64_t c)
    __attribute__((alias("gj_median3_u64")));
