/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4476: deque push-back tail advance index.
 *
 * Surface (unique symbols):
 *   size_t gj_deque_push_back_u(size_t tail, size_t cap);
 *     - Advance the back/tail cursor one slot after insert:
 *       (tail + 1) % cap. When cap == 0, return 0 (degenerate ring;
 *       avoid modulo by zero).
 *   size_t __gj_deque_push_back_u  (alias)
 *   __libcgj_batch4476_marker = "libcgj-batch4476"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_deque_u32_push_back (batch438) /
 * gj_deque_u32_push_back_u (batch3323) — pure index only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4476_marker[] = "libcgj-batch4476";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4476_back(size_t nTail, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nTail + 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_push_back_u - new back index after a push-back cursor step.
 *
 * tail: current back/tail write index
 * cap:  ring capacity in slots
 *
 * Returns (tail + 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_deque_push_back_u(size_t nTail, size_t nCap)
{
	(void)NULL;
	return b4476_back(nTail, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_deque_push_back_u(size_t nTail, size_t nCap)
    __attribute__((alias("gj_deque_push_back_u")));
