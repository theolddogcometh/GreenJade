/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1209: in-place clamp of a uint32 array.
 *
 * Surface (unique symbols):
 *   void gj_u32_clamp_arr(uint32_t *a, size_t n, uint32_t lo, uint32_t hi);
 *     — For each a[i], clamp into [lo, hi] when lo <= hi.
 *       If lo > hi, each element is forced to hi (degenerate).
 *       NULL a or n == 0 → no-op.
 *   void __gj_u32_clamp_arr  (alias)
 *   __libcgj_batch1209_marker = "libcgj-batch1209"
 *
 * Distinct from scalar gj_clamp_* — array form unique name; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1209_marker[] = "libcgj-batch1209";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_clamp_arr — clamp each element of a[0..n) into [lo, hi].
 */
void
gj_u32_clamp_arr(uint32_t *pA, size_t cN, uint32_t u32Lo, uint32_t u32Hi)
{
	size_t i;
	uint32_t u32LoEff;
	uint32_t u32HiEff;

	if (pA == NULL || cN == 0u) {
		return;
	}

	if (u32Lo <= u32Hi) {
		u32LoEff = u32Lo;
		u32HiEff = u32Hi;
	} else {
		/* Degenerate interval: force to hi. */
		u32LoEff = u32Hi;
		u32HiEff = u32Hi;
	}

	for (i = 0u; i < cN; i++) {
		uint32_t u32V;

		u32V = pA[i];
		if (u32V < u32LoEff) {
			u32V = u32LoEff;
		} else if (u32V > u32HiEff) {
			u32V = u32HiEff;
		}
		pA[i] = u32V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_clamp_arr(uint32_t *pA, size_t cN, uint32_t u32Lo,
    uint32_t u32Hi)
    __attribute__((alias("gj_u32_clamp_arr")));
