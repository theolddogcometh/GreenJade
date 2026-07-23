/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3332: binary min-heap peek root (minimum)
 * for uint32_t (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_heap_u32_peek_u(const uint32_t *a, size_t n, uint32_t *out);
 *     - Non-destructive read of the minimum (root). On success writes
 *       a[0] to *out and returns 0. Empty heap, NULL a, or NULL out
 *       returns -1 without writing.
 *   int __gj_heap_u32_peek_u  (alias)
 *   __libcgj_batch3332_marker = "libcgj-batch3332"
 *
 * Exclusive continuum CREATE-ONLY (3331-3340). Distinct from
 * gj_topk_peek_min (batch895), gj_heap_u32_pop_u (batch3331), and
 * gj_stack_u32_peek_u (batch2669) - unique gj_heap_u32_peek_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3332_marker[] = "libcgj-batch3332";

/* ---- freestanding helpers ---------------------------------------------- */

/* Read root into *pOut when heap is non-empty; else FAIL. */
static int
b3332_peek(const uint32_t *pA, size_t cN, uint32_t *pOut)
{
	if (pA == NULL || pOut == NULL || cN == 0u) {
		return -1;
	}
	*pOut = pA[0];
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_u32_peek_u - non-destructive read of the min-heap root.
 *
 * a:   heap base (NULL -> FAIL)
 * n:   live element count (0 -> FAIL)
 * out: destination for a[0] (NULL -> FAIL)
 *
 * Does not modify the heap. Returns 0 (OK) or -1 (FAIL).
 * No parent wires. Does not call libc.
 */
int
gj_heap_u32_peek_u(const uint32_t *a, size_t n, uint32_t *out)
{
	(void)NULL;
	return b3332_peek(a, n, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_heap_u32_peek_u(const uint32_t *a, size_t n, uint32_t *out)
    __attribute__((alias("gj_heap_u32_peek_u")));
