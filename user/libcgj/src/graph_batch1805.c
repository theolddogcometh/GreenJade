/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1805: byte-ring free-space query.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u8_space(size_t cap, size_t count);
 *     — Return free slots: cap - count when count <= cap; else 0
 *       (corrupt / overfull occupancy treated as no free space).
 *       cap == 0 → 0.
 *   size_t __gj_ring_u8_space  (alias)
 *   __libcgj_batch1805_marker = "libcgj-batch1805"
 *
 * Distinct from gj_ring_space (batch117 struct ring) — unique u8 free-
 * space surface for the batch281-style head/tail/count design.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1805_marker[] = "libcgj-batch1805";

/* ---- freestanding helpers ---------------------------------------------- */

/* Free slots given capacity and occupancy; 0 if corrupt/overfull. */
static size_t
b1805_free(size_t cap, size_t count)
{
	if (cap == 0u) {
		return 0u;
	}
	if (count > cap) {
		return 0u;
	}
	return cap - count;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_space — free capacity of a count-based u8 ring.
 *
 * cap:   total slot count of the caller buffer
 * count: live occupancy (0..cap when valid)
 *
 * Returns free slots. Overfull count or zero cap yields 0.
 */
size_t
gj_ring_u8_space(size_t cap, size_t count)
{
	return b1805_free(cap, count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u8_space(size_t cap, size_t count)
    __attribute__((alias("gj_ring_u8_space")));
