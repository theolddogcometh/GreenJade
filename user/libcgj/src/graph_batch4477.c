/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4477: modular index wrap-increment.
 *
 * Surface (unique symbols):
 *   size_t gj_idx_wrap_inc_u(size_t i, size_t cap);
 *     - Increment with wrap: (i + 1) % cap. When cap == 0, return 0
 *       (degenerate ring; avoid modulo by zero).
 *   size_t __gj_idx_wrap_inc_u  (alias)
 *   __libcgj_batch4477_marker = "libcgj-batch4477"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_ring_next_u (batch4351) — unique
 * gj_idx_wrap_inc_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4477_marker[] = "libcgj-batch4477";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4477_inc(size_t nI, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nI + 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_wrap_inc_u - modular wrap-increment of index i under cap.
 *
 * i:   current index
 * cap: modular modulus / ring capacity
 *
 * Returns (i + 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_idx_wrap_inc_u(size_t nI, size_t nCap)
{
	(void)NULL;
	return b4477_inc(nI, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_wrap_inc_u(size_t nI, size_t nCap)
    __attribute__((alias("gj_idx_wrap_inc_u")));
