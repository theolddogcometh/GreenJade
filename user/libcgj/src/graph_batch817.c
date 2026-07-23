/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch817: freestanding in-place exclusive prefix
 * sum over a uint32_t array.
 *
 * Surface (unique symbols):
 *   void gj_u32_prefix_sum(uint32_t *a, size_t n);
 *     — In-place exclusive prefix: after the call,
 *         a'[0] = 0
 *         a'[i] = a[0] + ... + a[i-1]   (i > 0)
 *       Sums wrap mod 2^32. a == NULL or n == 0 → no-op.
 *   void __gj_u32_prefix_sum  (alias)
 *   __libcgj_batch817_marker = "libcgj-batch817"
 *
 * Distinct from gj_u32_sum (batch609, scalar reduce) and sibling
 * delta/run-length surfaces — unique gj_u32_prefix_sum only; no
 * multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch817_marker[] = "libcgj-batch817";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_prefix_sum — in-place exclusive prefix sum of a[0..n).
 *
 * a: array of at least n elements (NULL → no-op)
 * n: element count (0 → no-op)
 *
 * Single forward pass keeps a running exclusive total: store the
 * previous sum, then accumulate the original a[i].
 */
void
gj_u32_prefix_sum(uint32_t *pA, size_t n)
{
	size_t i;
	uint32_t u32Prev;
	uint32_t u32Cur;

	if (pA == NULL || n == 0u) {
		return;
	}

	u32Prev = 0u;
	for (i = 0u; i < n; i++) {
		u32Cur = pA[i];
		pA[i] = u32Prev;
		u32Prev = u32Prev + u32Cur;
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_u32_prefix_sum(uint32_t *pA, size_t n)
    __attribute__((alias("gj_u32_prefix_sum")));
