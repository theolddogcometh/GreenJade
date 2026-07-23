/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch818: freestanding in-place first-order delta
 * of a uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_delta(uint32_t *a, size_t n);
 *     — In-place: a[i] = a[i] - a[i-1] for i > 0 (mod 2^32 wrap).
 *       a[0] is left unchanged. a == NULL or n < 2 → no-op.
 *   void __gj_u32_delta  (alias)
 *   __libcgj_batch818_marker = "libcgj-batch818"
 *
 * Inverse-shaped sibling of gj_u32_prefix_sum (batch817) under a
 * unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch818_marker[] = "libcgj-batch818";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_delta — in-place first differences of a[0..n).
 *
 * a: array of at least n elements (NULL → no-op)
 * n: element count (n < 2 → no-op; a[0] unchanged)
 *
 * Forward pass: remember the previous original value so each step
 * can write a[i] - prev without clobbering the next basis.
 */
void
gj_u32_delta(uint32_t *pA, size_t n)
{
	size_t i;
	uint32_t u32Prev;
	uint32_t u32Cur;

	if (pA == NULL || n < 2u) {
		return;
	}

	u32Prev = pA[0];
	for (i = 1u; i < n; i++) {
		u32Cur = pA[i];
		pA[i] = u32Cur - u32Prev;
		u32Prev = u32Cur;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_delta(uint32_t *pA, size_t n)
    __attribute__((alias("gj_u32_delta")));
