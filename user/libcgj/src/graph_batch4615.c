/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4615: dense index offset of element (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_idx_of_u(size_t base, size_t elem);
 *     - Relative index of elem from base: elem - base when
 *       elem >= base, else ~(size_t)0 (not-found sentinel).
 *   size_t __gj_idx_of_u  (alias)
 *   __libcgj_batch4615_marker = "libcgj-batch4615"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_idx_of_u
 * surface only; no multi-def. Index-space only (size_t params as
 * indices); no pointer arithmetic. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4615_marker[] = "libcgj-batch4615";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4615_idx_of(size_t szBase, size_t szElem)
{
	if (szElem >= szBase) {
		return szElem - szBase;
	}
	return ~(size_t)0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_of_u - relative dense index of elem from base.
 *
 * base: origin index
 * elem: element index
 *
 * Returns elem - base when elem >= base; otherwise ~(size_t)0 as a
 * not-found / out-of-range sentinel. No parent wires.
 */
size_t
gj_idx_of_u(size_t szBase, size_t szElem)
{
	(void)NULL;
	return b4615_idx_of(szBase, szElem);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_of_u(size_t szBase, size_t szElem)
    __attribute__((alias("gj_idx_of_u")));
