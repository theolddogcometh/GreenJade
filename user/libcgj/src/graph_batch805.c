/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch805: freestanding uint16_t array fill.
 *
 * Surface (unique symbols):
 *   void gj_u16_fill(uint16_t *dst, size_t n, uint16_t v);
 *     — Write v into each of the first n elements of dst.
 *       dst == NULL or n == 0 → no-op.
 *   void __gj_u16_fill  (alias)
 *   __libcgj_batch805_marker = "libcgj-batch805"
 *
 * Distinct from gj_u32_fill (batch764) / gj_u64_fill (batch765) /
 * gj_i32_fill (batch766). Unique gj_u16_fill surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch805_marker[] = "libcgj-batch805";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_fill — set dst[0..n) to the constant value v.
 *
 * dst: destination array of at least n uint16_t (NULL → no-op)
 * n:   element count
 * v:   fill value
 */
void
gj_u16_fill(uint16_t *pDst, size_t n, uint16_t u16V)
{
	size_t i;

	if (pDst == NULL || n == 0u) {
		return;
	}
	for (i = 0u; i < n; i++) {
		pDst[i] = u16V;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u16_fill(uint16_t *pDst, size_t n, uint16_t u16V)
    __attribute__((alias("gj_u16_fill")));
