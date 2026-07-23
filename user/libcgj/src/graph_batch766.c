/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch766: freestanding int32_t array fill.
 *
 * Surface (unique symbols):
 *   void gj_i32_fill(int32_t *dst, size_t n, int32_t v);
 *     — Write v into each of the first n elements of dst.
 *       dst == NULL or n == 0 → no-op.
 *   void __gj_i32_fill  (alias)
 *   __libcgj_batch766_marker = "libcgj-batch766"
 *
 * Distinct from gj_u32_fill (batch764) / gj_u64_fill (batch765).
 * Unique gj_i32_fill surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch766_marker[] = "libcgj-batch766";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_fill — set dst[0..n) to the constant value v.
 *
 * dst: destination array of at least n int32_t (NULL → no-op)
 * n:   element count
 * v:   fill value
 */
void
gj_i32_fill(int32_t *pDst, size_t n, int32_t i32V)
{
	size_t i;

	if (pDst == NULL || n == 0u) {
		return;
	}
	for (i = 0u; i < n; i++) {
		pDst[i] = i32V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_i32_fill(int32_t *pDst, size_t n, int32_t i32V)
    __attribute__((alias("gj_i32_fill")));
