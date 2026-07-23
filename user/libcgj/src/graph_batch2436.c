/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2436: uint32_t ring free-space query (_u surface).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u32_space_u(size_t cap, size_t count);
 *     — Return free slots: cap - count when count <= cap; else 0
 *       (corrupt / overfull occupancy treated as no free space).
 *       cap == 0 → 0.
 *   size_t __gj_ring_u32_space_u  (alias)
 *   __libcgj_batch2436_marker = "libcgj-batch2436"
 *
 * Distinct from gj_ring_space (batch117) and gj_ring_u8_space (batch1805).
 * Unique _u surface for the count-based u32 advanced exclusive wave.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 * No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2436_marker[] = "libcgj-batch2436";

/* ---- freestanding helpers ---------------------------------------------- */

/* Free slots given capacity and occupancy; 0 if corrupt/overfull. */
static size_t
b2436_free(size_t cap, size_t count)
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
 * gj_ring_u32_space_u — free capacity of a count-based u32 ring.
 *
 * cap:   total slot count of the caller buffer
 * count: live occupancy (0..cap when valid)
 *
 * Returns free slots. Overfull count or zero cap yields 0.
 */
size_t
gj_ring_u32_space_u(size_t cap, size_t count)
{
	return b2436_free(cap, count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u32_space_u(size_t cap, size_t count)
    __attribute__((alias("gj_ring_u32_space_u")));
