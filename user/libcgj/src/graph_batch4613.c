/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4613: list node index end predicate (_u).
 *
 * Surface (unique symbols):
 *   int gj_list_is_end_u(size_t i, size_t n);
 *     - True (1) when index i is at or past length n (i >= n); else 0.
 *   int __gj_list_is_end_u  (alias)
 *   __libcgj_batch4613_marker = "libcgj-batch4613"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_list_is_end_u
 * surface only; no multi-def. Distinct from gj_list_empty_p (batch2332)
 * and gj_idx_valid_u (batch4616). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4613_marker[] = "libcgj-batch4613";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b4613_list_is_end(size_t szI, size_t szN)
{
	if (szI >= szN) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_is_end_u - true when a dense list index is at/past end.
 *
 * i: current index
 * n: list length (exclusive end)
 *
 * Returns 1 when i >= n, else 0. No parent wires.
 */
int
gj_list_is_end_u(size_t szI, size_t szN)
{
	(void)NULL;
	return b4613_list_is_end(szI, szN);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_list_is_end_u(size_t szI, size_t szN)
    __attribute__((alias("gj_list_is_end_u")));
