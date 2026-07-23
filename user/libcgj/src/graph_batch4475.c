/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4475: deque push-front head retreat index.
 *
 * Surface (unique symbols):
 *   size_t gj_deque_push_front_u(size_t head, size_t cap);
 *     - Retreat the front/head cursor one slot before insert:
 *       (head + cap - 1) % cap. When cap == 0, return 0 (degenerate
 *       ring; avoid modulo by zero).
 *   size_t __gj_deque_push_front_u  (alias)
 *   __libcgj_batch4475_marker = "libcgj-batch4475"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_deque_u32_push_front (batch438) /
 * gj_deque_u32_push_front_u (batch3322) — pure index only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4475_marker[] = "libcgj-batch4475";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4475_front(size_t nHead, size_t nCap)
{
	if (nCap == 0u) {
		return 0u;
	}
	return (nHead + nCap - 1u) % nCap;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_push_front_u - new front index after a push-front cursor step.
 *
 * head: current front/head index
 * cap:  ring capacity in slots
 *
 * Returns (head + cap - 1) % cap, or 0 when cap is 0.
 * Self-contained; no parent wires.
 */
size_t
gj_deque_push_front_u(size_t nHead, size_t nCap)
{
	(void)NULL;
	return b4475_front(nHead, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_deque_push_front_u(size_t nHead, size_t nCap)
    __attribute__((alias("gj_deque_push_front_u")));
