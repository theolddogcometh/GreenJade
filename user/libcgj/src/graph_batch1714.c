/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1714: merge two half-open uint64 ranges if they
 * overlap or abut.
 *
 * Surface (unique symbols):
 *   int gj_range_u64_merge_if_overlap(uint64_t alo, uint64_t ahi,
 *                                     uint64_t blo, uint64_t bhi,
 *                                     uint64_t *out_lo, uint64_t *out_hi);
 *     — Half-open ranges [alo, ahi) and [blo, bhi). If both are non-empty
 *       and they overlap or abut (max(lo) <= min(hi)), writes the merged
 *       hull [min(lo), max(hi)) and returns 1. Otherwise returns 0 and
 *       leaves outs unset. NULL out-pointers skipped.
 *   int __gj_range_u64_merge_if_overlap  (alias)
 *   __libcgj_batch1714_marker = "libcgj-batch1714"
 *
 * Distinct from gj_range_u64_intersect (batch1713) which never merges
 * abutting peers. Interval / range exclusive. Unique surface only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1714_marker[] = "libcgj-batch1714";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1714_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

static uint64_t
b1714_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_u64_merge_if_overlap — merge if overlap or abut.
 *
 * Overlap: max(alo, blo) < min(ahi, bhi)
 * Abut:    max(alo, blo) == min(ahi, bhi)  (e.g. [0,5) + [5,10))
 * Merge hull: [min(alo, blo), max(ahi, bhi))
 */
int
gj_range_u64_merge_if_overlap(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi, uint64_t *pOutLo, uint64_t *pOutHi)
{
	uint64_t u64TouchLo;
	uint64_t u64TouchHi;
	uint64_t u64Lo;
	uint64_t u64Hi;

	if (u64ALo >= u64AHi || u64BLo >= u64BHi) {
		return 0;
	}

	u64TouchLo = b1714_max_u64(u64ALo, u64BLo);
	u64TouchHi = b1714_min_u64(u64AHi, u64BHi);
	/* Gap when touch_lo > touch_hi; abut when equal; overlap when less. */
	if (u64TouchLo > u64TouchHi) {
		return 0;
	}

	u64Lo = b1714_min_u64(u64ALo, u64BLo);
	u64Hi = b1714_max_u64(u64AHi, u64BHi);

	if (pOutLo != NULL) {
		*pOutLo = u64Lo;
	}
	if (pOutHi != NULL) {
		*pOutHi = u64Hi;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_u64_merge_if_overlap(uint64_t u64ALo, uint64_t u64AHi,
    uint64_t u64BLo, uint64_t u64BHi, uint64_t *pOutLo, uint64_t *pOutHi)
    __attribute__((alias("gj_range_u64_merge_if_overlap")));
