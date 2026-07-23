/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3321: freestanding uint32_t deque init (_u).
 *
 * Surface (unique symbols):
 *   void gj_deque_u32_init_u(size_t *head, size_t *tail, size_t *count);
 *     - Zero head, tail, and count on caller-owned slots. NULL any
 *       pointer -> no-op. Leaves buffer contents untouched.
 *   void __gj_deque_u32_init_u  (alias)
 *   __libcgj_batch3321_marker = "libcgj-batch3321"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330): deque_u32_init_u,
 * deque_u32_push_front_u, deque_u32_push_back_u, deque_u32_pop_front_u,
 * deque_u32_pop_back_u, deque_u32_len_u, heap_u32_sift_up_u,
 * heap_u32_sift_down_u, heap_u32_push_u, batch_id_3330.
 * Distinct from gj_queue_u32_init (batch437) — unique _u surface only;
 * no multi-def. No parent wires.
 *
 * Ring layout shared by this wave (count-based, same as batch438):
 *   head  — next back-write index (mod cap)
 *   tail  — front / next-read index (mod cap)
 *   count — elements currently stored (0..cap)
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3321_marker[] = "libcgj-batch3321";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deque_u32_init_u - reset deque cursors on caller storage.
 *
 * head / tail / count: caller-owned size_t slots (NULL any -> no-op).
 * Leaves the element buffer untouched (caller may reuse or zero).
 * Does not call libc. No parent wires.
 */
void
gj_deque_u32_init_u(size_t *pHead, size_t *pTail, size_t *pCount)
{
	if (pHead == NULL || pTail == NULL || pCount == NULL) {
		return;
	}
	*pHead = 0u;
	*pTail = 0u;
	*pCount = 0u;
	(void)NULL;
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_deque_u32_init_u(size_t *pHead, size_t *pTail, size_t *pCount)
    __attribute__((alias("gj_deque_u32_init_u")));
