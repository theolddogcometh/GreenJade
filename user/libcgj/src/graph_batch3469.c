/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3469: uint32_t ring length/occupancy query
 * (_u surface).
 *
 * Surface (unique symbols):
 *   size_t gj_ring_u32_len_u(size_t count);
 *     — Return the live occupancy of a count-based u32 ring. Passes
 *       through the caller-owned count watermark. Typed length surface
 *       for graph nodes that probe occupancy without exposing raw
 *       fields. Always returns the input count (by value).
 *   size_t __gj_ring_u32_len_u  (alias)
 *   __libcgj_batch3469_marker = "libcgj-batch3469"
 *
 * CREATE-ONLY exclusive continuum wave (3461-3470). Distinct from
 * gj_ring_u32_count_u (batch2435) by public name — len is the length-
 * oriented probe alias surface. Unique _u surface; no multi-def. No
 * parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3469_marker[] = "libcgj-batch3469";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of occupancy (keeps TU non-trivial for -Werror). */
static size_t
b3469_id(size_t c)
{
	return c;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ring_u32_len_u — report live length/occupancy of a count-based u32 ring.
 *
 * count: caller-owned occupancy watermark (0..cap for a valid ring)
 *
 * Returns count unchanged. Does not validate against capacity (capacity
 * is not available on this surface). Does not call libc. No parent wires.
 */
size_t
gj_ring_u32_len_u(size_t count)
{
	(void)NULL;
	return b3469_id(count);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_ring_u32_len_u(size_t count)
    __attribute__((alias("gj_ring_u32_len_u")));
