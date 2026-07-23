/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1712: half-open uint64 range overlap predicate.
 *
 * Surface (unique symbols):
 *   int gj_range_u64_overlap_p(uint64_t alo, uint64_t ahi,
 *                              uint64_t blo, uint64_t bhi);
 *     — Half-open ranges [alo, ahi) and [blo, bhi). Returns 1 if the
 *       intersection is non-empty, else 0. Empty inputs → 0. Abutting
 *       ranges (ahi == blo) do not overlap.
 *   int __gj_range_u64_overlap_p  (alias)
 *   __libcgj_batch1712_marker = "libcgj-batch1712"
 *
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1712_marker[] = "libcgj-batch1712";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1712_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

static uint64_t
b1712_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_u64_overlap_p — half-open range overlap predicate.
 *
 * True iff max(alo, blo) < min(ahi, bhi).
 */
int
gj_range_u64_overlap_p(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
{
	uint64_t u64Lo;
	uint64_t u64Hi;

	if (u64ALo >= u64AHi || u64BLo >= u64BHi) {
		return 0;
	}

	u64Lo = b1712_max_u64(u64ALo, u64BLo);
	u64Hi = b1712_min_u64(u64AHi, u64BHi);
	return (u64Lo < u64Hi) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_u64_overlap_p(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
    __attribute__((alias("gj_range_u64_overlap_p")));
