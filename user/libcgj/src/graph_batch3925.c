/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3925: binary min-heap peek root (minimum)
 * for uint64_t (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_heap_u64_peek_u(const uint64_t *a, size_t n, uint64_t *out);
 *     - Non-destructive read of the minimum (root). On success writes
 *       a[0] to *out and returns 0. Empty heap, NULL a, or NULL out
 *       returns -1 without writing.
 *   int __gj_heap_u64_peek_u  (alias)
 *   __libcgj_batch3925_marker = "libcgj-batch3925"
 *
 * Exclusive continuum CREATE-ONLY (3921-3930). Distinct from
 * gj_heap_u32_peek_u (batch3332), gj_topk_peek_min (batch895), and
 * gj_heap_u64_pop_u (batch3924) - unique gj_heap_u64_peek_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3925_marker[] = "libcgj-batch3925";

/* ---- freestanding helpers ---------------------------------------------- */

/* Read root into *pOut when heap is non-empty; else FAIL. */
static int
b3925_peek(const uint64_t *pA, size_t cN, uint64_t *pOut)
{
	if (pA == NULL || pOut == NULL || cN == 0u) {
		return -1;
	}
	*pOut = pA[0];
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_heap_u64_peek_u - non-destructive read of the min-heap root.
 *
 * a:   heap base (NULL -> FAIL)
 * n:   live element count (0 -> FAIL)
 * out: destination for a[0] (NULL -> FAIL)
 *
 * Does not modify the heap. Returns 0 (OK) or -1 (FAIL).
 * No parent wires. Does not call libc.
 */
int
gj_heap_u64_peek_u(const uint64_t *a, size_t n, uint64_t *out)
{
	(void)NULL;
	return b3925_peek(a, n, out);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_heap_u64_peek_u(const uint64_t *a, size_t n, uint64_t *out)
    __attribute__((alias("gj_heap_u64_peek_u")));
