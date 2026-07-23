/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1808: byte-ring empty predicate.
 *
 * Surface (unique symbols):
 *   int gj_ring_u8_empty_p(size_t count);
 *     — Return 1 if the count-based u8 ring is empty (count == 0),
 *       else 0.
 *   int __gj_ring_u8_empty_p  (alias)
 *   __libcgj_batch1808_marker = "libcgj-batch1808"
 *
 * Ring design (count-based, same as batch281): empty when count == 0.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1808_marker[] = "libcgj-batch1808";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy is zero. */
static int
b1808_empty(size_t count)
{
	if (count == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_empty_p — 1 if count-based u8 ring holds no bytes.
 *
 * count: live occupancy
 *
 * Returns 1 when empty, 0 otherwise.
 */
int
gj_ring_u8_empty_p(size_t count)
{
	return b1808_empty(count);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ring_u8_empty_p(size_t count)
    __attribute__((alias("gj_ring_u8_empty_p")));
