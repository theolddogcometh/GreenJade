/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3162: half-open range union-need predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_range_union_need_u(uint64_t alo, uint64_t ahi,
 *                             uint64_t blo, uint64_t bhi);
 *     - Half-open ranges [alo, ahi) and [blo, bhi). Returns 1 if both
 *       are non-empty and they overlap or abut (max(lo) <= min(hi)), so
 *       a single merged hull is the natural union; else 0. Abutting
 *       peers (ahi == blo) need union; a strict gap does not.
 *   int __gj_range_union_need_u  (alias)
 *   __libcgj_batch3162_marker = "libcgj-batch3162"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_range_u64_merge_if_overlap (batch1714) which also writes the hull —
 * unique gj_range_union_need_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3162_marker[] = "libcgj-batch3162";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3162_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

static uint64_t
b3162_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_union_need_u - true if two half-open ranges should be united.
 *
 * Overlap: max(alo, blo) < min(ahi, bhi)
 * Abut:    max(alo, blo) == min(ahi, bhi)
 * Gap:     max(alo, blo) > min(ahi, bhi)  → return 0
 *
 * Empty inputs (lo >= hi) never need union. No parent wires.
 */
int
gj_range_union_need_u(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
{
	uint64_t u64TouchLo;
	uint64_t u64TouchHi;

	(void)NULL;
	if (u64ALo >= u64AHi || u64BLo >= u64BHi) {
		return 0;
	}

	u64TouchLo = b3162_max_u64(u64ALo, u64BLo);
	u64TouchHi = b3162_min_u64(u64AHi, u64BHi);
	/* Gap when touch_lo > touch_hi; abut when equal; overlap when less. */
	if (u64TouchLo > u64TouchHi) {
		return 0;
	}
	return 1;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_union_need_u(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
    __attribute__((alias("gj_range_union_need_u")));
