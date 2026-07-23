/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1211: in-place clamp of an int32 array.
 *
 * Surface (unique symbols):
 *   void gj_i32_clamp_arr(int32_t *a, size_t n, int32_t lo, int32_t hi);
 *     — For each a[i], clamp into [lo, hi] when lo <= hi.
 *       If lo > hi, each element is forced to hi (degenerate).
 *       NULL a or n == 0 → no-op.
 *   void __gj_i32_clamp_arr  (alias)
 *   __libcgj_batch1211_marker = "libcgj-batch1211"
 *
 * Distinct from scalar gj_i32_clamp (batch644) — array form; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1211_marker[] = "libcgj-batch1211";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_clamp_arr — clamp each element of a[0..n) into [lo, hi].
 */
void
gj_i32_clamp_arr(int32_t *pA, size_t cN, int32_t i32Lo, int32_t i32Hi)
{
	size_t i;
	int32_t i32LoEff;
	int32_t i32HiEff;

	if (pA == NULL || cN == 0u) {
		return;
	}

	if (i32Lo <= i32Hi) {
		i32LoEff = i32Lo;
		i32HiEff = i32Hi;
	} else {
		i32LoEff = i32Hi;
		i32HiEff = i32Hi;
	}

	for (i = 0u; i < cN; i++) {
		int32_t i32V;

		i32V = pA[i];
		if (i32V < i32LoEff) {
			i32V = i32LoEff;
		} else if (i32V > i32HiEff) {
			i32V = i32HiEff;
		}
		pA[i] = i32V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_clamp_arr(int32_t *pA, size_t cN, int32_t i32Lo, int32_t i32Hi)
    __attribute__((alias("gj_i32_clamp_arr")));
