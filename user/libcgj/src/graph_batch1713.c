/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1713: half-open uint64 range intersection.
 *
 * Surface (unique symbols):
 *   int gj_range_u64_intersect(uint64_t alo, uint64_t ahi,
 *                              uint64_t blo, uint64_t bhi,
 *                              uint64_t *out_lo, uint64_t *out_hi);
 *     — Half-open ranges [alo, ahi) and [blo, bhi). Writes the
 *       intersection [max(lo), min(hi)) to out pointers (NULL outs
 *       skipped). Returns 1 if non-empty intersection, else 0 (outs
 *       left unset on empty when written).
 *   int __gj_range_u64_intersect  (alias)
 *   __libcgj_batch1713_marker = "libcgj-batch1713"
 *
 * Distinct from gj_range_u64_overlap_p (batch1712) / merge (batch1714).
 * Interval / range exclusive (half-open). Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no float. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1713_marker[] = "libcgj-batch1713";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1713_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

static uint64_t
b1713_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_u64_intersect — half-open range intersection.
 *
 * Intersection is [max(alo, blo), min(ahi, bhi)). Empty when lo >= hi.
 */
int
gj_range_u64_intersect(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi, uint64_t *pOutLo, uint64_t *pOutHi)
{
	uint64_t u64Lo;
	uint64_t u64Hi;

	u64Lo = b1713_max_u64(u64ALo, u64BLo);
	u64Hi = b1713_min_u64(u64AHi, u64BHi);

	if (u64Lo >= u64Hi) {
		return 0;
	}

	if (pOutLo != NULL) {
		*pOutLo = u64Lo;
	}
	if (pOutHi != NULL) {
		*pOutHi = u64Hi;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_u64_intersect(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi, uint64_t *pOutLo, uint64_t *pOutHi)
    __attribute__((alias("gj_range_u64_intersect")));
