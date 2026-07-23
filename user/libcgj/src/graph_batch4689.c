/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4689: buffer slice emptiness predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_slice_empty_u(size_t start, size_t end);
 *     - True (1) when half-open slice [start, end) is empty or inverted
 *       (end <= start); else 0.
 *   int __gj_slice_empty_u  (alias)
 *   __libcgj_batch4689_marker = "libcgj-batch4689"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_empty_u surface only;
 * no multi-def. Distinct from gj_list_is_end_u (batch4613). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4689_marker[] = "libcgj-batch4689";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4689_slice_empty(size_t szStart, size_t szEnd)
{
	if (szEnd <= szStart) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_empty_u - true when half-open [start, end) is empty/inverted.
 *
 * start: inclusive lower bound
 * end:   exclusive upper bound
 *
 * Returns 1 when end <= start, else 0. No parent wires.
 */
int
gj_slice_empty_u(size_t szStart, size_t szEnd)
{
	(void)NULL;
	return b4689_slice_empty(szStart, szEnd);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slice_empty_u(size_t szStart, size_t szEnd)
    __attribute__((alias("gj_slice_empty_u")));
