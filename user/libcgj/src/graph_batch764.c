/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch764: freestanding uint32_t array fill.
 *
 * Surface (unique symbols):
 *   void gj_u32_fill(uint32_t *dst, size_t n, uint32_t v);
 *     — Write v into each of the first n elements of dst.
 *       dst == NULL or n == 0 → no-op.
 *   void __gj_u32_fill  (alias)
 *   __libcgj_batch764_marker = "libcgj-batch764"
 *
 * Unique gj_u32_fill surface only; no multi-def with gj_u64_fill /
 * gj_i32_fill (sibling batches).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch764_marker[] = "libcgj-batch764";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_fill — set dst[0..n) to the constant value v.
 *
 * dst: destination array of at least n uint32_t (NULL → no-op)
 * n:   element count
 * v:   fill value
 */
void
gj_u32_fill(uint32_t *pDst, size_t n, uint32_t u32V)
{
	size_t i;

	if (pDst == NULL || n == 0u) {
		return;
	}
	for (i = 0u; i < n; i++) {
		pDst[i] = u32V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_fill(uint32_t *pDst, size_t n, uint32_t u32V)
    __attribute__((alias("gj_u32_fill")));
