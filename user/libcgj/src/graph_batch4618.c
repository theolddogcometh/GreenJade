/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4618: dense index modular wrap (_u).
 *
 * Surface (unique symbols):
 *   size_t gj_idx_wrap_u(size_t i, size_t n);
 *     - Wrap index i into [0, n) via unsigned remainder: i % n when
 *       n != 0, else 0 (soft guard).
 *   size_t __gj_idx_wrap_u  (alias)
 *   __libcgj_batch4618_marker = "libcgj-batch4618"
 *
 * Exclusive continuum CREATE-ONLY (4611-4620: list_next_u, list_prev_u,
 * list_is_end_u, list_count_u, idx_of_u, idx_valid_u, idx_clamp_u,
 * idx_wrap_u, idx_advance_u, batch_id_4620). Unique gj_idx_wrap_u
 * surface only; no multi-def. Distinct from gj_u32_mod_wrap_u
 * (batch4075) and gj_idx_clamp_u (batch4617). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4618_marker[] = "libcgj-batch4618";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4618_idx_wrap(size_t szI, size_t szN)
{
	if (szN == (size_t)0u) {
		return (size_t)0u;
	}
	return szI % szN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_wrap_u - wrap a dense index into [0, n) by modular remainder.
 *
 * i: index to wrap
 * n: modulus / period (0 → 0 soft guard)
 *
 * Returns i % n, or 0 when n is zero. No parent wires.
 */
size_t
gj_idx_wrap_u(size_t szI, size_t szN)
{
	(void)NULL;
	return b4618_idx_wrap(szI, szN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_wrap_u(size_t szI, size_t szN)
    __attribute__((alias("gj_idx_wrap_u")));
