/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4478: modular index wrap-decrement.
 *
 * Surface (unique symbols):
 *   size_t gj_idx_wrap_dec_u(size_t i, size_t cap);
 *     - Decrement with wrap: (i + cap - 1) % cap. When cap == 0, return 0
 *       (degenerate ring; avoid modulo by zero).
 *   size_t __gj_idx_wrap_dec_u  (alias)
 *   __libcgj_batch4478_marker = "libcgj-batch4478"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_ring_prev_u (batch4352) — unique
 * gj_idx_wrap_dec_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4478_marker[] = "libcgj-batch4478";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4478_dec(size_t nI, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nI + nCap - 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_idx_wrap_dec_u - modular wrap-decrement of index i under cap.
 *
 * i:   current index
 * cap: modular modulus / ring capacity
 *
 * Returns (i + cap - 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_idx_wrap_dec_u(size_t nI, size_t nCap)
{
	(void)NULL;
	return b4478_dec(nI, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_idx_wrap_dec_u(size_t nI, size_t nCap)
    __attribute__((alias("gj_idx_wrap_dec_u")));
