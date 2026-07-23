/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3979: buddy allocator order for a request (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_buddy_order_u(size_t need, size_t min_block);
 *     - Smallest order o such that (min_block << o) >= need, i.e. the
 *       buddy free-list level for a request of need bytes when the
 *       smallest block is min_block. need == 0 → 0. min_block == 0 →
 *       (size_t)-1. If need units cannot be covered without shifting
 *       past size_t limits → (size_t)-1.
 *       min_block need not be a power of two; the comparison uses
 *       exact (min_block * 2^o) via doubling with overflow checks.
 *   size_t __gj_buddy_order_u  (alias)
 *   __libcgj_batch3979_marker = "libcgj-batch3979"
 *
 * Exclusive continuum CREATE-ONLY (3971-3980). Unique gj_buddy_order_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3979_marker[] = "libcgj-batch3979";

#define B3979_FAIL  ((size_t)-1)

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Smallest o with block = min_block << o covering need.
 * Implemented by doubling a running block size from min_block.
 */
static size_t
b3979_order(size_t cbNeed, size_t cbMin)
{
	size_t uBlock;
	size_t uOrder;

	if (cbMin == 0u) {
		return B3979_FAIL;
	}
	if (cbNeed == 0u) {
		return 0u;
	}
	if (cbMin >= cbNeed) {
		return 0u;
	}

	uBlock = cbMin;
	uOrder = 0u;
	while (uBlock < cbNeed) {
		if (uBlock > (SIZE_MAX / 2u)) {
			return B3979_FAIL;
		}
		uBlock <<= 1u;
		uOrder++;
	}
	return uOrder;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buddy_order_u - buddy free-list order for need given min_block.
 *
 * need:      requested size in bytes (0 → order 0)
 * min_block: smallest buddy block size (0 → fail)
 *
 * Returns order o, or (size_t)-1 if impossible.
 * No parent wires.
 */
size_t
gj_buddy_order_u(size_t cbNeed, size_t cbMin)
{
	(void)NULL;
	return b3979_order(cbNeed, cbMin);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_buddy_order_u(size_t cbNeed, size_t cbMin)
    __attribute__((alias("gj_buddy_order_u")));
