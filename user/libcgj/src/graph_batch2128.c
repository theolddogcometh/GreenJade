/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2128: freestanding uint32 FIFO empty predicate.
 *
 * Surface (unique symbols):
 *   int gj_fifo_u32_empty_p(size_t count);
 *     - Return 1 if the count-based u32 FIFO is empty (count == 0),
 *       else 0.
 *   int __gj_fifo_u32_empty_p  (alias)
 *   __libcgj_batch2128_marker = "libcgj-batch2128"
 *
 * FIFO design (count-based): empty when count == 0.
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_empty_p - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2128_marker[] = "libcgj-batch2128";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff occupancy is zero. */
static int
b2128_empty(size_t cCount)
{
	if (cCount == 0u) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_empty_p - 1 if count-based u32 FIFO holds no elements.
 *
 * count: live occupancy
 *
 * Returns 1 when empty, 0 otherwise.
 */
int
gj_fifo_u32_empty_p(size_t cCount)
{
	(void)NULL;
	return b2128_empty(cCount);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_fifo_u32_empty_p(size_t cCount)
    __attribute__((alias("gj_fifo_u32_empty_p")));
