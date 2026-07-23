/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4473: queue enqueue write-cursor advance.
 *
 * Surface (unique symbols):
 *   size_t gj_queue_en_idx_u(size_t tail, size_t cap);
 *     - Advance the enqueue (write) cursor one slot: (tail + 1) % cap.
 *       When cap == 0, return 0 (degenerate ring; avoid modulo by zero).
 *   size_t __gj_queue_en_idx_u  (alias)
 *   __libcgj_batch4473_marker = "libcgj-batch4473"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_queue_u32_push (batch437) and
 * gj_ring_next_u (batch4351) — unique queue enqueue index surface;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4473_marker[] = "libcgj-batch4473";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4473_en(size_t nTail, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nTail + 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_queue_en_idx_u - next queue write cursor after enqueue.
 *
 * tail: current enqueue (write) index
 * cap:  ring capacity in slots
 *
 * Returns (tail + 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_queue_en_idx_u(size_t nTail, size_t nCap)
{
	(void)NULL;
	return b4473_en(nTail, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_queue_en_idx_u(size_t nTail, size_t nCap)
    __attribute__((alias("gj_queue_en_idx_u")));
