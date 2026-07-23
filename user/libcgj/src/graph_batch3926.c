/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3926: freestanding uint32 priority-queue
 * "better" comparator for min-PQ (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_pq_u32_better_u(uint32_t a, uint32_t b);
 *     - Returns 1 if priority a is strictly better than b for a min
 *       priority queue (a < b), else 0. Equal keys are not better.
 *   int __gj_pq_u32_better_u  (alias)
 *   __libcgj_batch3926_marker = "libcgj-batch3926"
 *
 * Exclusive continuum CREATE-ONLY (3921-3930). Aligns with min-heap
 * order used by heap_u64_*_u in this wave (strictly smaller is better).
 * Unique gj_pq_u32_better_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3926_marker[] = "libcgj-batch3926";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if a is strictly better (smaller) than b for min-PQ. */
static int
b3926_better(uint32_t uA, uint32_t uB)
{
	if (uA < uB) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_pq_u32_better_u - min-PQ priority compare: is a better than b?
 *
 * a, b: unsigned 32-bit priorities (smaller is better)
 *
 * Returns 1 when a is strictly less than b, else 0. Pure data predicate;
 * does not mutate state or call other surfaces. No parent wires.
 */
int
gj_pq_u32_better_u(uint32_t a, uint32_t b)
{
	(void)NULL;
	return b3926_better(a, b);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_pq_u32_better_u(uint32_t a, uint32_t b)
    __attribute__((alias("gj_pq_u32_better_u")));
