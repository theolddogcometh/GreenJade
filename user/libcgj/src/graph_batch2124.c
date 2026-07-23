/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2124: freestanding uint32 FIFO occupancy query.
 *
 * Surface (unique symbols):
 *   size_t gj_fifo_u32_count(size_t count);
 *     - Return the live occupancy of a count-based u32 FIFO. Passes
 *       through the caller-owned count watermark. Useful as a typed
 *       surface / graph node for occupancy probes without exposing raw
 *       fields. Always returns the input count (by value).
 *   size_t __gj_fifo_u32_count  (alias)
 *   __libcgj_batch2124_marker = "libcgj-batch2124"
 *
 * Post-2100 fifo_u32 exclusive wave (2121-2130). Distinct from
 * gj_ring_u8_count / gj_fifo_depth - unique surface; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2124_marker[] = "libcgj-batch2124";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of occupancy (keeps TU non-trivial for -Werror). */
static size_t
b2124_id(size_t cCount)
{
	return cCount;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fifo_u32_count - report live occupancy of a count-based u32 FIFO.
 *
 * count: caller-owned occupancy watermark (0..cap for a valid FIFO)
 *
 * Returns count unchanged. Does not validate against capacity (capacity
 * is not available on this surface); callers pair with gj_fifo_u32_space
 * / full_p when needed.
 */
size_t
gj_fifo_u32_count(size_t cCount)
{
	(void)NULL;
	return b2124_id(cCount);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_fifo_u32_count(size_t cCount)
    __attribute__((alias("gj_fifo_u32_count")));
