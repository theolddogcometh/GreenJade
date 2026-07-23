/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3163: half-open range intersection predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_range_intersect_p_u(uint64_t alo, uint64_t ahi,
 *                              uint64_t blo, uint64_t bhi);
 *     - Half-open ranges [alo, ahi) and [blo, bhi). Returns 1 if the
 *       intersection is non-empty, else 0. Empty inputs → 0. Abutting
 *       ranges (ahi == blo) do not intersect.
 *   int __gj_range_intersect_p_u  (alias)
 *   __libcgj_batch3163_marker = "libcgj-batch3163"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_range_u64_overlap_p (batch1712) / gj_range_u64_intersect (batch1713)
 * — unique gj_range_intersect_p_u surface only; no multi-def. No parent
 * wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3163_marker[] = "libcgj-batch3163";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b3163_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

static uint64_t
b3163_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_range_intersect_p_u - half-open range intersection predicate.
 *
 * True iff max(alo, blo) < min(ahi, bhi). No parent wires.
 */
int
gj_range_intersect_p_u(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
{
	uint64_t u64Lo;
	uint64_t u64Hi;

	(void)NULL;
	if (u64ALo >= u64AHi || u64BLo >= u64BHi) {
		return 0;
	}

	u64Lo = b3163_max_u64(u64ALo, u64BLo);
	u64Hi = b3163_min_u64(u64AHi, u64BHi);
	return (u64Lo < u64Hi) ? 1 : 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_range_intersect_p_u(uint64_t u64ALo, uint64_t u64AHi, uint64_t u64BLo,
    uint64_t u64BHi)
    __attribute__((alias("gj_range_intersect_p_u")));
