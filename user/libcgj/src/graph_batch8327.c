/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8327: qsort needed-for-n predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_qsort_need_u_8327(size_t n);
 *     - Return 1 if a qsort of n elements is needed (n >= 2); else 0.
 *       Soft qsort precondition helper (empty / singleton are no-ops).
 *   uint32_t __gj_qsort_need_u_8327  (alias)
 *   __libcgj_batch8327_marker = "libcgj-batch8327"
 *
 * Exclusive continuum CREATE-ONLY (8321-8330: qsort/bsearch compare
 * stubs). Unique gj_qsort_need_u_8327 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8327_marker[] = "libcgj-batch8327";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff n >= 2 (sort has work to do). */
static uint32_t
b8327_need(size_t cN)
{
	if (cN >= 2u) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_qsort_need_u_8327 - 1 if qsort of n elements is needed.
 *
 * n: element count of the candidate array
 *
 * Returns 1 when n >= 2, else 0. Does not sort. No parent wires.
 */
uint32_t
gj_qsort_need_u_8327(size_t cN)
{
	(void)NULL;
	return b8327_need(cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_qsort_need_u_8327(size_t cN)
    __attribute__((alias("gj_qsort_need_u_8327")));
