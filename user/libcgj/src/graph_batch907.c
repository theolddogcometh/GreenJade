/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch907: mode of a sorted uint32 array.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_mode_sorted(const uint32_t *a, size_t n);
 *     — Return the most common value in a[0..n), which must be sorted
 *       non-decreasing. Ties break to the smallest value (first mode
 *       in sort order). NULL a or n == 0 → 0.
 *   uint32_t __gj_u32_mode_sorted  (alias)
 *   __libcgj_batch907_marker = "libcgj-batch907"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch907_marker[] = "libcgj-batch907";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_mode_sorted — most frequent value in a sorted uint32 run.
 *
 * Single linear scan: track the current run length and the best
 * (longest) run seen so far. Equal-length ties keep the earlier
 * (smaller) value because the array is sorted ascending.
 */
uint32_t
gj_u32_mode_sorted(const uint32_t *a, size_t n)
{
	size_t i;
	size_t cRun;
	size_t cBest;
	uint32_t uRun;
	uint32_t uBest;

	if (a == NULL || n == 0u) {
		return 0u;
	}

	uBest = a[0];
	cBest = 1u;
	uRun = a[0];
	cRun = 1u;

	for (i = 1u; i < n; i++) {
		if (a[i] == uRun) {
			cRun++;
		} else {
			if (cRun > cBest) {
				cBest = cRun;
				uBest = uRun;
			}
			uRun = a[i];
			cRun = 1u;
		}
	}
	if (cRun > cBest) {
		uBest = uRun;
	}
	return uBest;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_mode_sorted(const uint32_t *a, size_t n)
    __attribute__((alias("gj_u32_mode_sorted")));
