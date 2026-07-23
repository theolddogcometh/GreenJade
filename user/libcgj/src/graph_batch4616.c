/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4616: dense index validity predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_idx_valid_u(size_t i, size_t n);
 *     - True (1) when index i lies in the half-open range [0, n)
 *       (i < n); else 0.
 *   int __gj_idx_valid_u  (alias)
 *   __libcgj_batch4616_marker = "libcgj-batch4616"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_idx_valid_u
 * surface only; no multi-def. Distinct from gj_list_is_end_u
 * (batch4613) which is the complementary i >= n predicate. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4616_marker[] = "libcgj-batch4616";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4616_idx_valid(size_t szI, size_t szN)
{
	if (szI < szN) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_valid_u - true when a dense index lies in [0, n).
 *
 * i: index to test
 * n: exclusive upper bound / length
 *
 * Returns 1 when i < n, else 0. No parent wires.
 */
int
gj_idx_valid_u(size_t szI, size_t szN)
{
	(void)NULL;
	return b4616_idx_valid(szI, szN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_idx_valid_u(size_t szI, size_t szN)
    __attribute__((alias("gj_idx_valid_u")));
