/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4686: buffer slice union upper bound (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slice_union_hi_u(size_t a1, size_t b1);
 *     - Exclusive upper bound of the union of two slices: max(a1, b1).
 *   size_t __gj_slice_union_hi_u  (alias)
 *   __libcgj_batch4686_marker = "libcgj-batch4686"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_union_hi_u surface
 * only; no multi-def. Pair with gj_slice_union_lo_u (batch4685).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4686_marker[] = "libcgj-batch4686";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4686_slice_union_hi(size_t szA1, size_t szB1)
{
	if (szA1 > szB1) {
		return szA1;
	}
	return szB1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_union_hi_u - exclusive upper bound of the union of two slices.
 *
 * a1: first slice exclusive end
 * b1: second slice exclusive end
 *
 * Returns max(a1, b1). Pair with gj_slice_union_lo_u for [lo, hi).
 * No parent wires.
 */
size_t
gj_slice_union_hi_u(size_t szA1, size_t szB1)
{
	(void)NULL;
	return b4686_slice_union_hi(szA1, szB1);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slice_union_hi_u(size_t szA1, size_t szB1)
    __attribute__((alias("gj_slice_union_hi_u")));
