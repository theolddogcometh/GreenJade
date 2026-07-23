/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4471: stack push index (count-as-top).
 *
 * Surface (unique symbols):
 *   size_t gj_stack_push_idx_u(size_t top, size_t cap);
 *     - top is the element count (0..cap). When top < cap, return
 *       top + 1 (successful push depth). When top >= cap, return cap
 *       (full; depth unchanged / saturated at capacity).
 *   size_t __gj_stack_push_idx_u  (alias)
 *   __libcgj_batch4471_marker = "libcgj-batch4471"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Distinct from gj_stack_u32_push (batch436) /
 * gj_u64_stack_push (batch675) — pure count index only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4471_marker[] = "libcgj-batch4471";

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b4471_push(size_t nTop, size_t nCap)
{
	if (nTop >= nCap) {
		return nCap;
	}
	return nTop + 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_stack_push_idx_u - new stack depth after a push attempt.
 *
 * top: current element count (top-of-stack depth)
 * cap: maximum capacity in slots
 *
 * Returns top + 1 when top < cap; otherwise cap (full).
 * Self-contained; no parent wires.
 */
size_t
gj_stack_push_idx_u(size_t nTop, size_t nCap)
{
	(void)NULL;
	return b4471_push(nTop, nCap);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_stack_push_idx_u(size_t nTop, size_t nCap)
    __attribute__((alias("gj_stack_push_idx_u")));
