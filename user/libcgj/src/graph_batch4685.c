/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4685: buffer slice union lower bound (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slice_union_lo_u(size_t a0, size_t b0);
 *     - Lower bound of the union of two slices: min(a0, b0).
 *   size_t __gj_slice_union_lo_u  (alias)
 *   __libcgj_batch4685_marker = "libcgj-batch4685"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_union_lo_u surface
 * only; no multi-def. Pair with gj_slice_union_hi_u (batch4686).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4685_marker[] = "libcgj-batch4685";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4685_slice_union_lo(size_t szA0, size_t szB0)
{
	if (szA0 < szB0) {
		return szA0;
	}
	return szB0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_union_lo_u - lower bound of the union of two slices.
 *
 * a0: first slice inclusive start
 * b0: second slice inclusive start
 *
 * Returns min(a0, b0). Pair with gj_slice_union_hi_u for [lo, hi).
 * No parent wires.
 */
size_t
gj_slice_union_lo_u(size_t szA0, size_t szB0)
{
	(void)NULL;
	return b4685_slice_union_lo(szA0, szB0);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slice_union_lo_u(size_t szA0, size_t szB0)
    __attribute__((alias("gj_slice_union_lo_u")));
