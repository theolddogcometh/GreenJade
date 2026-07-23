/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3326: freestanding uint32_t deque length (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_deque_u32_len_u(const size_t *count);
 *     - Return *count when count is non-NULL, else 0. Reports occupancy
 *       of a count-based deque (batch3321-3325 ring layout).
 *   size_t __gj_deque_u32_len_u  (alias)
 *   __libcgj_batch3326_marker = "libcgj-batch3326"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330). Distinct from
 * gj_deque_depth_ring (batch1304) — unique len_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3326_marker[] = "libcgj-batch3326";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_len_u - report live element count of a count-based deque.
 *
 * count: pointer to the occupancy field owned by the caller (NULL -> 0).
 *
 * Returns *count, or 0 if count is NULL. Does not inspect head/tail or
 * buffer. No parent wires.
 */
size_t
gj_deque_u32_len_u(const size_t *pCount)
{
	if (pCount == NULL) {
		return 0u;
	}
	(void)NULL;
	return *pCount;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_deque_u32_len_u(const size_t *pCount)
    __attribute__((alias("gj_deque_u32_len_u")));
