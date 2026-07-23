/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4683: buffer slice index clamp (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slice_clamp_u(size_t i, size_t start, size_t end);
 *     - Clamp i into the last valid index of half-open [start, end):
 *       non-empty → clamp into [start, end - 1]; empty/inverted → start.
 *   size_t __gj_slice_clamp_u  (alias)
 *   __libcgj_batch4683_marker = "libcgj-batch4683"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_clamp_u surface only;
 * no multi-def. Distinct from gj_idx_clamp_u (batch4617) which is
 * fixed-origin [0, n). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4683_marker[] = "libcgj-batch4683";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4683_slice_clamp(size_t szI, size_t szStart, size_t szEnd)
{
	size_t szLast;

	if (szEnd <= szStart) {
		return szStart;
	}
	szLast = szEnd - (size_t)1u;
	if (szI < szStart) {
		return szStart;
	}
	if (szI > szLast) {
		return szLast;
	}
	return szI;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_clamp_u - clamp i into the last valid slot of [start, end).
 *
 * i:     index to clamp
 * start: inclusive lower bound
 * end:   exclusive upper bound
 *
 * Returns start when the slice is empty/inverted; otherwise confining
 * i into [start, end - 1]. No parent wires.
 */
size_t
gj_slice_clamp_u(size_t szI, size_t szStart, size_t szEnd)
{
	(void)NULL;
	return b4683_slice_clamp(szI, szStart, szEnd);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slice_clamp_u(size_t szI, size_t szStart, size_t szEnd)
    __attribute__((alias("gj_slice_clamp_u")));
