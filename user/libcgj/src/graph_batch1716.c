/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1716: half-open int64 range overlap predicate.
 *
 * Surface (unique symbols):
 *   int gj_range_i64_overlap_p(int64_t alo, int64_t ahi,
 *                              int64_t blo, int64_t bhi);
 *     — Half-open ranges [alo, ahi) and [blo, bhi). Returns 1 if the
 *       intersection is non-empty, else 0. Empty inputs → 0. Abutting
 *       ranges (ahi == blo) do not overlap.
 *   int __gj_range_i64_overlap_p  (alias)
 *   __libcgj_batch1716_marker = "libcgj-batch1716"
 *
 * Distinct from gj_range_u64_overlap_p (batch1712). Interval / range
 * exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1716_marker[] = "libcgj-batch1716";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b1716_max_i64(int64_t i64A, int64_t i64B)
{
	return (i64A > i64B) ? i64A : i64B;
}

static int64_t
b1716_min_i64(int64_t i64A, int64_t i64B)
{
	return (i64A < i64B) ? i64A : i64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_i64_overlap_p — half-open range overlap predicate.
 *
 * True iff max(alo, blo) < min(ahi, bhi).
 */
int
gj_range_i64_overlap_p(int64_t i64ALo, int64_t i64AHi, int64_t i64BLo,
    int64_t i64BHi)
{
	int64_t i64Lo;
	int64_t i64Hi;

	if (i64ALo >= i64AHi || i64BLo >= i64BHi) {
		return 0;
	}

	i64Lo = b1716_max_i64(i64ALo, i64BLo);
	i64Hi = b1716_min_i64(i64AHi, i64BHi);
	return (i64Lo < i64Hi) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_i64_overlap_p(int64_t i64ALo, int64_t i64AHi, int64_t i64BLo,
    int64_t i64BHi)
    __attribute__((alias("gj_range_i64_overlap_p")));
