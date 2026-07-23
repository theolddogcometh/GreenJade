/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch895: top-k min-heap peek root (minimum).
 *
 * Complements gj_topk_init / gj_topk_offer (batch497). Unique symbol.
 * Root of the min-heap is the smallest of the retained top-k values.
 *
 * Surface (unique symbols):
 *   uint64_t gj_topk_peek_min(const uint64_t *h, size_t n);
 *     — Return h[0] when n > 0; else 0. NULL h → 0.
 *   uint64_t __gj_topk_peek_min  (alias)
 *   __libcgj_batch895_marker = "libcgj-batch895"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch895_marker[] = "libcgj-batch895";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_topk_peek_min — read the current minimum of a top-k min-heap.
 *
 * Does not modify the heap. Empty heap (n == 0) or NULL h returns 0.
 * Note: a legitimate stored value of 0 is indistinguishable from empty
 * when n is not also checked by the caller.
 */
uint64_t
gj_topk_peek_min(const uint64_t *h, size_t n)
{
	if (h == NULL || n == 0u) {
		return 0ull;
	}
	return h[0];
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_topk_peek_min(const uint64_t *h, size_t n)
    __attribute__((alias("gj_topk_peek_min")));
