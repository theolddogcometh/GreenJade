/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2435: uint32_t ring occupancy query (_u surface).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u32_count_u(size_t count);
 *     — Return the live occupancy of a count-based u32 ring. Passes
 *       through the caller-owned count watermark. Useful as a typed
 *       surface / graph node for occupancy probes without exposing raw
 *       fields. Always returns the input count (by value).
 *   size_t __gj_ring_u32_count_u  (alias)
 *   __libcgj_batch2435_marker = "libcgj-batch2435"
 *
 * Distinct from gj_ring_u8_count (batch1804) and gj_ring_avail (batch117).
 * Unique _u surface for the count-based u32 advanced exclusive wave.
 *
 * Ring advanced exclusive wave (2431–2439). Clean-room freestanding pure C
 * (integer only). Compiles with -ffreestanding -msse2 -Wall -Wextra
 * -Werror. No malloc, no errno, no libc. No third-party source copied.
 * No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2435_marker[] = "libcgj-batch2435";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of occupancy (keeps TU non-trivial for -Werror). */
static size_t
b2435_id(size_t c)
{
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_count_u — report live occupancy of a count-based u32 ring.
 *
 * count: caller-owned occupancy watermark (0..cap for a valid ring)
 *
 * Returns count unchanged. Does not validate against capacity (capacity
 * is not available on this surface); callers pair with
 * gj_ring_u32_space_u / full_u_p when needed.
 */
size_t
gj_ring_u32_count_u(size_t count)
{
	(void)NULL;
	return b2435_id(count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u32_count_u(size_t count)
    __attribute__((alias("gj_ring_u32_count_u")));
