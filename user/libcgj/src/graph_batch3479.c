/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3479: freestanding uint64 queue length query
 * (_u exclusive surface).
 *
 * Surface (unique symbols):
 *   size_t gj_queue_u64_len_u(size_t n);
 *     - Return the live occupancy of a count-based u64 queue. Passes
 *       through the caller-owned count watermark. Useful as a typed
 *       surface / graph node for occupancy probes without exposing raw
 *       fields. Always returns the input n (by value).
 *   size_t __gj_queue_u64_len_u  (alias)
 *   __libcgj_batch3479_marker = "libcgj-batch3479"
 *
 * Exclusive continuum CREATE-ONLY (3471-3480). Distinct from
 * gj_queue_u32_len_u (batch2665) by public queue_u64_len_u name —
 * unique surface; no multi-def. No parent wires. Caller buffers only
 * (count is a by-value watermark).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3479_marker[] = "libcgj-batch3479";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity pass-through of occupancy (keeps TU non-trivial for -Werror). */
static size_t
b3479_id(size_t cN)
{
	return cN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_u64_len_u - report live occupancy of a count-based u64 queue.
 *
 * n: caller-owned occupancy watermark (0..cap for a valid queue)
 *
 * Returns n unchanged. Does not validate against capacity (capacity is
 * not available on this surface); callers pair with push/pop full/empty
 * checks when needed.
 */
size_t
gj_queue_u64_len_u(size_t cN)
{
	(void)NULL;
	return b3479_id(cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_queue_u64_len_u(size_t cN)
    __attribute__((alias("gj_queue_u64_len_u")));
