/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2125: freestanding uint32 FIFO free-space query.
 *
 * Surface (unique symbols):
 *   size_t gj_fifo_u32_space(size_t cap, size_t count);
 *     - Return free slots: cap - count when count <= cap; else 0
 *       (corrupt / overfull occupancy treated as no free space).
 *       cap == 0 -> 0.
 *   size_t __gj_fifo_u32_space  (alias)
 *   __libcgj_batch2125_marker = "libcgj-batch2125"
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_space / gj_fifo_space - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2125_marker[] = "libcgj-batch2125";

/* ---- freestanding helpers ---------------------------------------------- */

/* Free slots given capacity and occupancy; 0 if corrupt/overfull. */
static size_t
b2125_free(size_t cCap, size_t cCount)
{
	if (cCap == 0u) {
		return 0u;
	}
	if (cCount > cCap) {
		return 0u;
	}
	return cCap - cCount;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_space - free capacity of a count-based u32 FIFO.
 *
 * cap:   total slot count of the caller buffer
 * count: live occupancy (0..cap when valid)
 *
 * Returns free slots. Overfull count or zero cap yields 0.
 */
size_t
gj_fifo_u32_space(size_t cCap, size_t cCount)
{
	(void)NULL;
	return b2125_free(cCap, cCount);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fifo_u32_space(size_t cCap, size_t cCount)
    __attribute__((alias("gj_fifo_u32_space")));
