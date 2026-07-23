/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch664: freestanding in-place reverse of a
 * bounded uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_reverse(uint32_t *a, size_t n);
 *     — Reverse a[0..n) in place via two-pointer swap.
 *       a == NULL or n < 2 → no-op.
 *   void __gj_u32_reverse  (alias)
 *   __libcgj_batch664_marker = "libcgj-batch664"
 *
 * Distinct from gj_reverse_u32 / __gj_reverse_u32 (graph_batch401) and
 * from gj_buf_rev / __gj_buf_rev (graph_batch435, byte-wise). Unique
 * gj_u32_reverse names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch664_marker[] = "libcgj-batch664";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_reverse — reverse a[0..n) in place via two-pointer swap.
 *
 * a: base of n uint32_t words (NULL → no-op)
 * n: element count (n < 2 → no-op; nothing to reverse)
 *
 * Mutates a[i] for i in [0, n). Does not call gj_reverse_u32 or libc.
 */
void
gj_u32_reverse(uint32_t *pA, size_t cN)
{
	size_t iLo;
	size_t iHi;
	uint32_t u32Tmp;

	if (pA == NULL || cN < 2u) {
		return;
	}

	iLo = 0u;
	iHi = cN - 1u;
	while (iLo < iHi) {
		u32Tmp = pA[iLo];
		pA[iLo] = pA[iHi];
		pA[iHi] = u32Tmp;
		iLo++;
		iHi--;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_reverse(uint32_t *pA, size_t cN)
    __attribute__((alias("gj_u32_reverse")));
