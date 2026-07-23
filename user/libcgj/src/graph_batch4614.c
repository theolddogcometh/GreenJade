/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4614: dense list/index span count (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_list_count_u(size_t start, size_t end);
 *     - Count elements in the half-open index span [start, end).
 *       Returns end - start when end >= start, else 0.
 *   size_t __gj_list_count_u  (alias)
 *   __libcgj_batch4614_marker = "libcgj-batch4614"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_list_count_u
 * surface only; no multi-def. Distinct from intrusive list length walks.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4614_marker[] = "libcgj-batch4614";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4614_list_count(size_t szStart, size_t szEnd)
{
	if (szEnd >= szStart) {
		return szEnd - szStart;
	}
	return (size_t)0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_count_u - count elements in the half-open index span [start, end).
 *
 * start: inclusive start index
 * end: exclusive end index
 *
 * Returns end - start when end >= start, otherwise 0. No parent wires.
 */
size_t
gj_list_count_u(size_t szStart, size_t szEnd)
{
	(void)NULL;
	return b4614_list_count(szStart, szEnd);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_count_u(size_t szStart, size_t szEnd)
    __attribute__((alias("gj_list_count_u")));
