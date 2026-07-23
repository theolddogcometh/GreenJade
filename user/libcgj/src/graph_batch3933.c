/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3933: freestanding uint64_t deque length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_deque_u64_len_u(const size_t *count);
 *     - Return *count when count is non-NULL, else 0. Reports occupancy
 *       of a count-based u64 deque (same ring layout as batch3931-3932).
 *   size_t __gj_deque_u64_len_u  (alias)
 *   __libcgj_batch3933_marker = "libcgj-batch3933"
 *
 * Exclusive continuum CREATE-ONLY (3931-3940). Distinct from
 * gj_deque_u32_len_u (batch3326), gj_deque_depth_ring (batch1304), and
 * gj_queue_u64_len_u (batch3479) — unique deque_u64_len_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3933_marker[] = "libcgj-batch3933";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u64_len_u - report live element count of a count-based u64 deque.
 *
 * count: pointer to the occupancy field owned by the caller (NULL -> 0).
 *
 * Returns *count, or 0 if count is NULL. Does not inspect head/tail or
 * buffer. No parent wires.
 */
size_t
gj_deque_u64_len_u(const size_t *pCount)
{
	if (pCount == NULL) {
		return 0u;
	}
	(void)NULL;
	return *pCount;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_deque_u64_len_u(const size_t *pCount)
    __attribute__((alias("gj_deque_u64_len_u")));
