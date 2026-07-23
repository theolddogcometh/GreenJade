/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8323: qsort/bsearch u32 descending (gt) cmp.
 *
 * Surface (unique symbols):
 *   int gj_cmp_u32_gt_8323(uint32_t a, uint32_t b);
 *     - Three-way descending compare: -1 if a > b, 0 if a == b, +1 if a < b.
 *       Reverse of lt order (greater-than / descending sort order).
 *   int __gj_cmp_u32_gt_8323  (alias)
 *   __libcgj_batch8323_marker = "libcgj-batch8323"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Unique gj_cmp_u32_gt_8323 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8323_marker[] = "libcgj-batch8323";

/* ---- freestanding helpers ---------------------------------------------- */

/* Three-way descending: a > b → -1, a == b → 0, a < b → +1. */
static int
b8323_cmp_gt(uint32_t u32A, uint32_t u32B)
{
	if (u32A > u32B) {
		return -1;
	}
	if (u32A < u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmp_u32_gt_8323 - descending three-way u32 compare (gt order).
 *
 * a, b: values under comparison
 *
 * Returns -1 / 0 / +1 for descending sort order. Overflow-safe.
 * No parent wires.
 */
int
gj_cmp_u32_gt_8323(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8323_cmp_gt(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cmp_u32_gt_8323(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_cmp_u32_gt_8323")));
