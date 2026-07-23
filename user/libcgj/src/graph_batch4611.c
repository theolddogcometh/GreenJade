/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4611: list node index next (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_list_next_u(size_t i);
 *     - Advance a dense list/node index by one step (i + 1).
 *   size_t __gj_list_next_u  (alias)
 *   __libcgj_batch4611_marker = "libcgj-batch4611"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_list_next_u
 * surface only; no multi-def. Distinct from intrusive list walkers
 * (batch233x). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4611_marker[] = "libcgj-batch4611";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4611_list_next(size_t szI)
{
	return szI + (size_t)1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_list_next_u - advance a dense list/node index by one.
 *
 * i: current index
 *
 * Returns i + 1. Unsigned wrap on overflow is intentional. No parent wires.
 */
size_t
gj_list_next_u(size_t szI)
{
	(void)NULL;
	return b4611_list_next(szI);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_list_next_u(size_t szI)
    __attribute__((alias("gj_list_next_u")));
