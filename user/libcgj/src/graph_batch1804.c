/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1804: byte-ring occupancy query.
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u8_count(size_t count);
 *     — Return the live occupancy of a count-based u8 ring. Passes
 *       through the caller-owned count watermark. Useful as a typed
 *       surface / graph node for occupancy probes without exposing raw
 *       fields. Always returns the input count (by value).
 *   size_t __gj_ring_u8_count  (alias)
 *   __libcgj_batch1804_marker = "libcgj-batch1804"
 *
 * Distinct from gj_ring_avail (batch117 struct ring) — unique u8 count
 * surface for the batch281-style head/tail/count design.
 *
 * Ring buffer advanced exclusive wave (1801–1809). Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1804_marker[] = "libcgj-batch1804";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of occupancy (keeps TU non-trivial for -Werror). */
static size_t
b1804_id(size_t c)
{
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u8_count — report live byte occupancy of a count-based ring.
 *
 * count: caller-owned occupancy watermark (0..cap for a valid ring)
 *
 * Returns count unchanged. Does not validate against capacity (capacity
 * is not available on this surface); callers pair with gj_ring_u8_space
 * / full_p when needed.
 */
size_t
gj_ring_u8_count(size_t count)
{
	return b1804_id(count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u8_count(size_t count)
    __attribute__((alias("gj_ring_u8_count")));
