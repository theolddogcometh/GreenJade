/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3159: half-open range overlap predicate (u).
 *
 * Surface (unique symbols):
 *   int gj_range_overlap_u(uint64_t a_lo, uint64_t a_hi,
 *                          uint64_t b_lo, uint64_t b_hi);
 *     - Return 1 if half-open ranges [a_lo, a_hi) and [b_lo, b_hi)
 *       overlap (non-empty intersection). Empty ranges (lo >= hi)
 *       never overlap. Equivalent to a_lo < b_hi && b_lo < a_hi when
 *       both ranges are non-empty.
 *   int __gj_range_overlap_u  (alias)
 *   __libcgj_batch3159_marker = "libcgj-batch3159"
 *
 * Milestone 3160 exclusive continuum CREATE-ONLY (3151-3160). Distinct
 * from gj_range_u64_contains (batch1711) — unique gj_range_overlap_u
 * surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3159_marker[] = "libcgj-batch3159";

/* ---- freestanding helpers ---------------------------------------------- */

/* Non-empty half-open [lo, hi). */
static int
b3159_nonempty(uint64_t u64Lo, uint64_t u64Hi)
{
	return (u64Lo < u64Hi) ? 1 : 0;
}

/*
 * True when both half-open ranges are non-empty and intersect.
 * Classic: a_lo < b_hi && b_lo < a_hi.
 */
static int
b3159_overlap(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi)
{
	if (b3159_nonempty(u64ALo, u64AHi) == 0) {
		return 0;
	}
	if (b3159_nonempty(u64BLo, u64BHi) == 0) {
		return 0;
	}
	if (u64ALo >= u64BHi) {
		return 0;
	}
	if (u64BLo >= u64AHi) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_overlap_u - true if half-open [a_lo, a_hi) overlaps [b_lo, b_hi).
 *
 * a_lo/a_hi: first half-open range
 * b_lo/b_hi: second half-open range
 *
 * Empty ranges never overlap. Returns 1 if intersection non-empty, else 0.
 * No parent wires.
 */
int
gj_range_overlap_u(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi)
{
	(void)NULL;
	return b3159_overlap(u64ALo, u64AHi, u64BLo, u64BHi);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_overlap_u(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi)
    __attribute__((alias("gj_range_overlap_u")));
