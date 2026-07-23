/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4480: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_4480(void);
 *     - Returns the compile-time graph batch number for this TU (4480).
 *   uint32_t __gj_batch_id_4480  (alias)
 *   __libcgj_batch4480_marker = "libcgj-batch4480"
 *
 * Exclusive continuum CREATE-ONLY (4471-4480: stack_push_idx_u,
 * stack_pop_idx_u, queue_en_idx_u, queue_de_idx_u, deque_push_front_u,
 * deque_push_back_u, idx_wrap_inc_u, idx_wrap_dec_u, capacity_pow2_u,
 * batch_id_4480). Does NOT redefine gj_batch_id / prior batch_id_* —
 * avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4480_marker[] = "libcgj-batch4480";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4480_id(void)
{
	return 4480u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_4480 - report this TU's graph batch number.
 *
 * Always returns 4480. Useful for link-time presence checks and runtime
 * graph revision probes without string compares on the marker.
 * Self-contained; no parent wires.
 */
uint32_t
gj_batch_id_4480(void)
{
	(void)NULL;
	return b4480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_4480(void)
    __attribute__((alias("gj_batch_id_4480")));
