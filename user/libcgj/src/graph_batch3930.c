/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3930: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3930(void);
 *     - Returns the compile-time graph batch number for this TU (3930).
 *   uint32_t __gj_batch_id_3930  (alias)
 *   __libcgj_batch3930_marker = "libcgj-batch3930"
 *
 * Exclusive continuum CREATE-ONLY (3921-3930): heap_u64_sift_up_u,
 * heap_u64_sift_down_u, heap_u64_push_u, heap_u64_pop_u, heap_u64_peek_u,
 * pq_u32_better_u, pq_u64_better_u, deque_u64_push_front_u,
 * deque_u64_push_back_u, batch_id_3930.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3930_marker[] = "libcgj-batch3930";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3930_id(void)
{
	return 3930u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3930 - report this TU's graph batch number.
 *
 * Always returns 3930.
 */
uint32_t
gj_batch_id_3930(void)
{
	(void)NULL;
	return b3930_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3930(void)
    __attribute__((alias("gj_batch_id_3930")));
