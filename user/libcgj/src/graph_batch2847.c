/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2847: ring index modular previous (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u8_prev_u(size_t idx, size_t cap);
 *     - Return the predecessor of idx modulo cap (wrap to cap - 1 when
 *       idx == 0). cap == 0 -> 0. Does not clamp idx itself.
 *   size_t __gj_ring_u8_prev_u  (alias)
 *   __libcgj_batch2847_marker = "libcgj-batch2847"
 *
 * Endian/ring exclusive wave (2841-2850). Pairs with gj_ring_u8_next_u
 * (batch2846). Unique _u surface; no multi-def. Does not touch buffer
 * storage.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2847_marker[] = "libcgj-batch2847";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular previous index under capacity cCap (cCap > 0). */
static size_t
b2847_prev(size_t iIdx, size_t cCap)
{
	if (iIdx == 0u) {
		return cCap - 1u;
	}
	return iIdx - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_prev_u - retreat a ring index by one with modular wrap.
 *
 * idx: current index (any size_t; not range-checked against cap)
 * cap: ring capacity (must be > 0 for a meaningful wrap)
 *
 * Returns (idx + cap - 1) mod cap when cap > 0; returns 0 when cap == 0.
 * Pure integer helper for reverse head/tail stepping.
 */
size_t
gj_ring_u8_prev_u(size_t iIdx, size_t cCap)
{
	(void)NULL;
	if (cCap == 0u) {
		return 0u;
	}
	return b2847_prev(iIdx, cCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u8_prev_u(size_t iIdx, size_t cCap)
    __attribute__((alias("gj_ring_u8_prev_u")));
