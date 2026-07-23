/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5288: GPU fence sequence identity (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_gpu_fence_seq_u(uint32_t seq);
 *     - Return the fence sequence value unchanged (identity / publish
 *       path for a monotonic GPU fence counter). Useful as a typed
 *       surface for fence seq materialization without side effects.
 *   uint32_t __gj_gpu_fence_seq_u  (alias)
 *   __libcgj_batch5288_marker = "libcgj-batch5288"
 *
 * Exclusive continuum CREATE-ONLY (5281-5290: gpu_ring_align_u,
 * gpu_ring_space_u, gpu_ring_used_u, gpu_ring_empty_u, gpu_ring_full_u,
 * gpu_ring_push_u, gpu_ring_pop_u, gpu_fence_seq_u, gpu_fence_signaled_u,
 * batch_id_5290). Unique gj_gpu_fence_seq_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5288_marker[] = "libcgj-batch5288";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5288_seq(uint32_t uSeq)
{
	return uSeq;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_gpu_fence_seq_u - materialize a GPU fence sequence value.
 *
 * seq: fence sequence number to publish / pass through
 *
 * Returns seq unchanged. Does not call libc. No parent wires.
 */
uint32_t
gj_gpu_fence_seq_u(uint32_t uSeq)
{
	(void)NULL;
	return b5288_seq(uSeq);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_gpu_fence_seq_u(uint32_t uSeq)
    __attribute__((alias("gj_gpu_fence_seq_u")));
