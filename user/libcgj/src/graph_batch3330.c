/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3330: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3330(void);
 *     - Returns the compile-time graph batch number for this TU (3330).
 *   uint32_t __gj_batch_id_3330  (alias)
 *   __libcgj_batch3330_marker = "libcgj-batch3330"
 *
 * Exclusive continuum CREATE-ONLY (3321-3330): deque_u32_init_u,
 * deque_u32_push_front_u, deque_u32_push_back_u, deque_u32_pop_front_u,
 * deque_u32_pop_back_u, deque_u32_len_u, heap_u32_sift_up_u,
 * heap_u32_sift_down_u, heap_u32_push_u, batch_id_3330.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3330_marker[] = "libcgj-batch3330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3330_id(void)
{
	return 3330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3330 - report this TU's graph batch number.
 *
 * Always returns 3330.
 */
uint32_t
gj_batch_id_3330(void)
{
	(void)NULL;
	return b3330_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3330(void)
    __attribute__((alias("gj_batch_id_3330")));
