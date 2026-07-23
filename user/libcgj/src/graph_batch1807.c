/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1807: byte-ring full predicate.
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_full_p(size_t cap, size_t count);
 *     — Return 1 if the count-based u8 ring is full (count >= cap and
 *       cap > 0), else 0. cap == 0 → 0 (no capacity is not "full").
 *   int __gj_ring_u8_full_p  (alias)
 *   __libcgj_batch1807_marker = "libcgj-batch1807"
 *
 * Ring design (count-based, same as batch281): full when count == cap.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1807_marker[] = "libcgj-batch1807";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff cap > 0 and occupancy covers every slot. */
static int
b1807_full(size_t cap, size_t count)
{
	if (cap == 0u) {
		return 0;
	}
	if (count >= cap) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_full_p — 1 if count-based u8 ring has no free slots.
 *
 * cap:   total slot count
 * count: live occupancy
 *
 * Returns 1 when full, 0 otherwise (including zero capacity).
 */
int
gj_ring_u8_full_p(size_t cap, size_t count)
{
	return b1807_full(cap, count);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_full_p(size_t cap, size_t count)
    __attribute__((alias("gj_ring_u8_full_p")));
