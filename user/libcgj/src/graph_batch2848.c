/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2848: ring occupancy full predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_full_p_u(size_t count, size_t cap);
 *     - Return 1 when a count-based ring is full (count >= cap and
 *       cap > 0). Return 0 when empty-capacity, not full, or count < cap.
 *   int __gj_ring_u8_full_p_u  (alias)
 *   __libcgj_batch2848_marker = "libcgj-batch2848"
 *
 * Endian/ring exclusive wave (2841-2850). Predicate companion for
 * batch281 gj_ring_u8_push (full when count == cap). Unique _u name;
 * no multi-def. Pure integer; no buffer access.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2848_marker[] = "libcgj-batch2848";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy meets or exceeds a positive capacity. */
static int
b2848_full(size_t cN, size_t cCap)
{
	if (cCap == 0u) {
		return 0;
	}
	if (cN >= cCap) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_full_p_u - true when a count-based u8 ring is full.
 *
 * count: live occupancy (0..cap under correct use)
 * cap:   ring capacity
 *
 * Returns 1 when cap > 0 and count >= cap, else 0.
 * Treats cap == 0 as not full (degenerate / unusable ring).
 */
int
gj_ring_u8_full_p_u(size_t cN, size_t cCap)
{
	(void)NULL;
	return b2848_full(cN, cCap);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_full_p_u(size_t cN, size_t cCap)
    __attribute__((alias("gj_ring_u8_full_p_u")));
