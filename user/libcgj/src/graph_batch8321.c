/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8321: qsort/bsearch u32 ascending (lt) cmp.
 *
 * Surface (unique symbols):
 *   int gj_cmp_u32_lt_8321(uint32_t a, uint32_t b);
 *     - Three-way ascending compare: -1 if a < b, 0 if a == b, +1 if a > b.
 *       Suitable as a typed qsort/bsearch order helper (less-than order).
 *   int __gj_cmp_u32_lt_8321  (alias)
 *   __libcgj_batch8321_marker = "libcgj-batch8321"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs — cmp_u32_lt, cmp_u32_eq, cmp_u32_gt, cmp_i32_sign,
 * bsearch_empty, bsearch_one, qsort_need, qsort_trivial, cmp_bytes,
 * batch_id_8330). Unique gj_cmp_u32_lt_8321 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8321_marker[] = "libcgj-batch8321";

/* ---- freestanding helpers ---------------------------------------------- */

/* Three-way: a < b → -1, a == b → 0, a > b → +1. */
static int
b8321_cmp_lt(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return -1;
	}
	if (u32A > u32B) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmp_u32_lt_8321 - ascending three-way u32 compare (lt order).
 *
 * a, b: values under comparison
 *
 * Returns -1 / 0 / +1. Overflow-safe (no subtraction). No parent wires.
 */
int
gj_cmp_u32_lt_8321(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b8321_cmp_lt(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_cmp_u32_lt_8321(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_cmp_u32_lt_8321")));
