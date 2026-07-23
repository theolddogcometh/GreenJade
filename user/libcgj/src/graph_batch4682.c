/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4682: buffer slice membership (_u).
 *
 * Surface (unique symbols):
 *   int gj_slice_contains_u(size_t start, size_t end, size_t i);
 *     - True (1) when index i lies in the half-open slice [start, end)
 *       (start <= i && i < end); else 0.
 *   int __gj_slice_contains_u  (alias)
 *   __libcgj_batch4682_marker = "libcgj-batch4682"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_contains_u surface
 * only; no multi-def. Distinct from gj_idx_valid_u (batch4616) which
 * is fixed-origin [0, n). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4682_marker[] = "libcgj-batch4682";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4682_slice_contains(size_t szStart, size_t szEnd, size_t szI)
{
	if (szI >= szStart && szI < szEnd) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_contains_u - true when i lies in half-open [start, end).
 *
 * start: inclusive lower bound
 * end:   exclusive upper bound
 * i:     index to test
 *
 * Returns 1 when start <= i < end, else 0. Empty/inverted slices
 * (end <= start) never contain. No parent wires.
 */
int
gj_slice_contains_u(size_t szStart, size_t szEnd, size_t szI)
{
	(void)NULL;
	return b4682_slice_contains(szStart, szEnd, szI);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slice_contains_u(size_t szStart, size_t szEnd, size_t szI)
    __attribute__((alias("gj_slice_contains_u")));
