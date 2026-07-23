/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4684: buffer slice range overlap (_u).
 *
 * Surface (unique symbols):
 *   int gj_slice_overlap_u(size_t a0, size_t a1, size_t b0, size_t b1);
 *     - True (1) when half-open ranges [a0, a1) and [b0, b1) overlap
 *       (both non-empty and a0 < b1 && b0 < a1); else 0.
 *   int __gj_slice_overlap_u  (alias)
 *   __libcgj_batch4684_marker = "libcgj-batch4684"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_overlap_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4684_marker[] = "libcgj-batch4684";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4684_slice_overlap(size_t szA0, size_t szA1, size_t szB0, size_t szB1)
{
	if (szA0 >= szA1) {
		return 0;
	}
	if (szB0 >= szB1) {
		return 0;
	}
	if (szA0 < szB1 && szB0 < szA1) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_overlap_u - true when two half-open slices overlap.
 *
 * a0, a1: first slice [a0, a1)
 * b0, b1: second slice [b0, b1)
 *
 * Returns 1 when both slices are non-empty and intersect, else 0.
 * Touching at an exclusive endpoint (e.g. a1 == b0) is not overlap.
 * No parent wires.
 */
int
gj_slice_overlap_u(size_t szA0, size_t szA1, size_t szB0, size_t szB1)
{
	(void)NULL;
	return b4684_slice_overlap(szA0, szA1, szB0, szB1);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slice_overlap_u(size_t szA0, size_t szA1, size_t szB0, size_t szB1)
    __attribute__((alias("gj_slice_overlap_u")));
