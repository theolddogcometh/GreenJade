/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5289: GPU fence signaled predicate (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_fence_signaled_u(uint32_t seq, uint32_t cur);
 *     - Returns 1 when the current fence counter has reached or passed
 *       the wait sequence (wrap-safe half-range compare on free-running
 *       uint32 counters): (int32_t)(cur - seq) >= 0. Else 0.
 *   uint32_t __gj_gpu_fence_signaled_u  (alias)
 *   __libcgj_batch5289_marker = "libcgj-batch5289"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_fence_signaled_u surface only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5289_marker[] = "libcgj-batch5289";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Wrap-safe: cur has reached seq when the signed difference is non-negative
 * (distance forward from seq to cur is less than half the uint32 domain).
 */
static uint32_t
b5289_signaled(uint32_t uSeq, uint32_t uCur)
{
	if ((uint32_t)(uCur - uSeq) <= 0x7fffffffu) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_fence_signaled_u - test whether a GPU fence wait is satisfied.
 *
 * seq: fence sequence the waiter is waiting for
 * cur: current fence counter value
 *
 * Returns 1 if cur has reached or passed seq under wrap-safe half-range
 * compare, else 0. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_fence_signaled_u(uint32_t uSeq, uint32_t uCur)
{
	(void)NULL;
	return b5289_signaled(uSeq, uCur);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_fence_signaled_u(uint32_t uSeq, uint32_t uCur)
    __attribute__((alias("gj_gpu_fence_signaled_u")));
