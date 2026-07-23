/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4612: list node index previous (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_list_prev_u(size_t i);
 *     - Step a dense list/node index backward by one; saturates at 0
 *       (i == 0 → 0, else i - 1).
 *   size_t __gj_list_prev_u  (alias)
 *   __libcgj_batch4612_marker = "libcgj-batch4612"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_list_prev_u
 * surface only; no multi-def. Distinct from gj_list_next_u (batch4611).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4612_marker[] = "libcgj-batch4612";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4612_list_prev(size_t szI)
{
	if (szI == (size_t)0u) {
		return (size_t)0u;
	}
	return szI - (size_t)1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_prev_u - step a dense list/node index backward by one.
 *
 * i: current index
 *
 * Returns 0 when i is 0, otherwise i - 1 (saturate at the head).
 * No parent wires.
 */
size_t
gj_list_prev_u(size_t szI)
{
	(void)NULL;
	return b4612_list_prev(szI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_prev_u(size_t szI)
    __attribute__((alias("gj_list_prev_u")));
