/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3534: mode of a uint32_t array (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_arr_u32_mode_u(const uint32_t *a, size_t n);
 *     - Return the most frequent value in a[0..n). Ties break to the
 *       smallest value. NULL a or n == 0 → 0. Does not mutate a;
 *       array need not be sorted.
 *   uint32_t __gj_arr_u32_mode_u  (alias)
 *   __libcgj_batch3534_marker = "libcgj-batch3534"
 *
 * Exclusive continuum CREATE-ONLY (3531-3540). Distinct from
 * gj_u32_mode_sorted (batch907 sorted run) and gj_u8_mode (batch1332)
 * — unique arr_u32_mode_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3534_marker[] = "libcgj-batch3534";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Mode via O(n^2) frequency scan. Ties → smaller value.
 * pA non-NULL, cN > 0. No allocation.
 */
static uint32_t
b3534_mode(const uint32_t *pA, size_t cN)
{
	size_t i;
	size_t j;
	size_t cBest;
	uint32_t uBest;

	uBest = pA[0];
	cBest = 0u;

	for (i = 0u; i < cN; i++) {
		size_t c;
		int bFirst;

		/* Only score first occurrence of each distinct value. */
		bFirst = 1;
		for (j = 0u; j < i; j++) {
			if (pA[j] == pA[i]) {
				bFirst = 0;
				break;
			}
		}
		if (!bFirst) {
			continue;
		}

		c = 0u;
		for (j = 0u; j < cN; j++) {
			if (pA[j] == pA[i]) {
				c++;
			}
		}
		if (c > cBest || (c == cBest && pA[i] < uBest)) {
			cBest = c;
			uBest = pA[i];
		}
	}
	return uBest;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_arr_u32_mode_u - most frequent uint32_t value in a[0..n).
 *
 * a: base (NULL → 0)
 * n: element count (0 → 0)
 *
 * Ties break to the smallest value. Does not call libc.
 */
uint32_t
gj_arr_u32_mode_u(const uint32_t *pA, size_t cN)
{
	(void)NULL;
	if (pA == NULL || cN == 0u) {
		return 0u;
	}
	return b3534_mode(pA, cN);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_arr_u32_mode_u(const uint32_t *pA, size_t cN)
    __attribute__((alias("gj_arr_u32_mode_u")));
