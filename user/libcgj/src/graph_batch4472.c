/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4472: stack pop index (count-as-top).
 *
 * Surface (unique symbols):
 *   size_t gj_stack_pop_idx_u(size_t top);
 *     - top is the element count. Returns top - 1 when top > 0;
 *       otherwise 0 (empty; depth floored at zero).
 *   size_t __gj_stack_pop_idx_u  (alias)
 *   __libcgj_batch4472_marker = "libcgj-batch4472"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_stack_u32_pop (batch436) — pure
 * count index only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4472_marker[] = "libcgj-batch4472";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4472_pop(size_t nTop)
{
	if (nTop == 0u) {
		return 0u;
	}
	return nTop - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_pop_idx_u - new stack depth after a pop attempt.
 *
 * top: current element count (top-of-stack depth)
 *
 * Returns top - 1 when top > 0; otherwise 0 (empty floor).
 * Self-contained; no parent wires.
 */
size_t
gj_stack_pop_idx_u(size_t nTop)
{
	(void)NULL;
	return b4472_pop(nTop);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_stack_pop_idx_u(size_t nTop)
    __attribute__((alias("gj_stack_pop_idx_u")));
