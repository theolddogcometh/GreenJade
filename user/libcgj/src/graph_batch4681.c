/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4681: buffer slice length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_slice_len_u(size_t start, size_t end);
 *     - Half-open slice length: end >= start ? end - start : 0.
 *   size_t __gj_slice_len_u  (alias)
 *   __libcgj_batch4681_marker = "libcgj-batch4681"
 *
 * Exclusive continuum CREATE-ONLY (4681-4690: slice_len_u,
 * slice_contains_u, slice_clamp_u, slice_overlap_u, slice_union_lo_u,
 * slice_union_hi_u, slice_intersect_lo_u, slice_intersect_hi_u,
 * slice_empty_u, batch_id_4690). Unique gj_slice_len_u surface only;
 * no multi-def. Distinct from gj_list_count_u (batch4614). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4681_marker[] = "libcgj-batch4681";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4681_slice_len(size_t szStart, size_t szEnd)
{
	if (szEnd >= szStart) {
		return szEnd - szStart;
	}
	return (size_t)0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slice_len_u - length of a half-open buffer slice [start, end).
 *
 * start: inclusive lower bound
 * end:   exclusive upper bound
 *
 * Returns end - start when end >= start, else 0 (inverted/empty).
 * No parent wires.
 */
size_t
gj_slice_len_u(size_t szStart, size_t szEnd)
{
	(void)NULL;
	return b4681_slice_len(szStart, szEnd);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_slice_len_u(size_t szStart, size_t szEnd)
    __attribute__((alias("gj_slice_len_u")));
