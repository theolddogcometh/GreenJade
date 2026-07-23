/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2438: uint32_t ring empty predicate (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_ring_u32_empty_u_p(size_t count);
 *     — Return 1 if the count-based u32 ring is empty (count == 0),
 *       else 0.
 *   int __gj_ring_u32_empty_u_p  (alias)
 *   __libcgj_batch2438_marker = "libcgj-batch2438"
 *
 * Ring design (count-based): empty when count == 0.
 * Distinct from gj_ring_u8_empty_p (batch1808). Unique _u surface.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 * No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2438_marker[] = "libcgj-batch2438";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy is zero. */
static int
b2438_empty(size_t count)
{
	if (count == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_empty_u_p — 1 if count-based u32 ring holds no elements.
 *
 * count: live occupancy
 *
 * Returns 1 when empty, 0 otherwise.
 */
int
gj_ring_u32_empty_u_p(size_t count)
{
	return b2438_empty(count);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u32_empty_u_p(size_t count)
    __attribute__((alias("gj_ring_u32_empty_u_p")));
