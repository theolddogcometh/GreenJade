/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2127: freestanding uint32 FIFO full predicate.
 *
 * Surface (unique symbols):
 *   int gj_fifo_u32_full_p(size_t cap, size_t count);
 *     - Return 1 if the count-based u32 FIFO is full (count >= cap and
 *       cap > 0), else 0. cap == 0 -> 0 (no capacity is not "full").
 *   int __gj_fifo_u32_full_p  (alias)
 *   __libcgj_batch2127_marker = "libcgj-batch2127"
 *
 * FIFO design (count-based): full when count == cap.
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_full_p - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2127_marker[] = "libcgj-batch2127";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff cap > 0 and occupancy covers every slot. */
static int
b2127_full(size_t cCap, size_t cCount)
{
	if (cCap == 0u) {
		return 0;
	}
	if (cCount >= cCap) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_full_p - 1 if count-based u32 FIFO has no free slots.
 *
 * cap:   total slot count
 * count: live occupancy
 *
 * Returns 1 when full, 0 otherwise (including zero capacity).
 */
int
gj_fifo_u32_full_p(size_t cCap, size_t cCount)
{
	(void)NULL;
	return b2127_full(cCap, cCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fifo_u32_full_p(size_t cCap, size_t cCount)
    __attribute__((alias("gj_fifo_u32_full_p")));
