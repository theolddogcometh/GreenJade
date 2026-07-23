/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4619: dense index modular advance (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_idx_advance_u(size_t i, size_t k, size_t n);
 *     - Advance index i by k steps, wrapping in [0, n): (i + k) % n
 *       when n != 0, else 0 (soft guard).
 *   size_t __gj_idx_advance_u  (alias)
 *   __libcgj_batch4619_marker = "libcgj-batch4619"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_idx_advance_u
 * surface only; no multi-def. Distinct from gj_idx_wrap_u (batch4618)
 * and gj_list_next_u (batch4611). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4619_marker[] = "libcgj-batch4619";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4619_idx_advance(size_t szI, size_t szK, size_t szN)
{
	if (szN == (size_t)0u) {
		return (size_t)0u;
	}
	/* Unsigned wrap of (i + k) is intentional before modulo. */
	return (szI + szK) % szN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_advance_u - advance a dense index by k steps with wrap in [0, n).
 *
 * i: starting index
 * k: steps to advance
 * n: modulus / period (0 → 0 soft guard)
 *
 * Returns (i + k) % n, or 0 when n is zero. No parent wires.
 */
size_t
gj_idx_advance_u(size_t szI, size_t szK, size_t szN)
{
	(void)NULL;
	return b4619_idx_advance(szI, szK, szN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_advance_u(size_t szI, size_t szK, size_t szN)
    __attribute__((alias("gj_idx_advance_u")));
