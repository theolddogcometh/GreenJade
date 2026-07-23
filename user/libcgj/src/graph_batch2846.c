/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2846: ring index modular next (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u8_next_u(size_t idx, size_t cap);
 *     - Return the successor of idx modulo cap (wrap to 0 when
 *       idx + 1 >= cap). cap == 0 -> 0. Does not clamp idx itself.
 *   size_t __gj_ring_u8_next_u  (alias)
 *   __libcgj_batch2846_marker = "libcgj-batch2846"
 *
 * Endian/ring exclusive wave (2841-2850). Cursor arithmetic companion
 * for batch281 gj_ring_u8_* (count-based head/tail). Unique _u name;
 * no multi-def. Does not touch buffer storage.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2846_marker[] = "libcgj-batch2846";

/* ---- freestanding helpers ---------------------------------------------- */

/* Modular next index under capacity cCap (cCap > 0). */
static size_t
b2846_next(size_t iIdx, size_t cCap)
{
	size_t iNext;

	iNext = iIdx + 1u;
	if (iNext >= cCap) {
		iNext = 0u;
	}
	return iNext;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_next_u - advance a ring index by one with modular wrap.
 *
 * idx: current index (any size_t; not range-checked against cap)
 * cap: ring capacity (must be > 0 for a meaningful wrap)
 *
 * Returns (idx + 1) mod cap when cap > 0; returns 0 when cap == 0.
 * Pure integer helper for head/tail advancement.
 */
size_t
gj_ring_u8_next_u(size_t iIdx, size_t cCap)
{
	(void)NULL;
	if (cCap == 0u) {
		return 0u;
	}
	return b2846_next(iIdx, cCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u8_next_u(size_t iIdx, size_t cCap)
    __attribute__((alias("gj_ring_u8_next_u")));
