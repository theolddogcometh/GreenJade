/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5284: GPU ring empty predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_ring_empty_u(uint32_t head, uint32_t tail);
 *     - Returns 1 when the modular GPU ring is empty (head == tail),
 *       else 0. Classic head/tail design: empty and full share the
 *       equal-cursor state only when empty; full uses the next-tail
 *       collision test (see gj_gpu_ring_full_u).
 *   uint32_t __gj_gpu_ring_empty_u  (alias)
 *   __libcgj_batch5284_marker = "libcgj-batch5284"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_ring_empty_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5284_marker[] = "libcgj-batch5284";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5284_empty(uint32_t uHead, uint32_t uTail)
{
	if (uHead == uTail) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_ring_empty_u - test whether a modular GPU ring is empty.
 *
 * head: next-pop index (consumer)
 * tail: next-push index (producer)
 *
 * Returns 1 when head == tail, else 0. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_gpu_ring_empty_u(uint32_t uHead, uint32_t uTail)
{
	(void)NULL;
	return b5284_empty(uHead, uTail);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_ring_empty_u(uint32_t uHead, uint32_t uTail)
    __attribute__((alias("gj_gpu_ring_empty_u")));
