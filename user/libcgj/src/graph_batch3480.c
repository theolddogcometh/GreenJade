/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3480: exclusive continuum batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_3480(void);
 *     - Returns the compile-time graph batch number for this TU (3480).
 *   uint32_t __gj_batch_id_3480  (alias)
 *   __libcgj_batch3480_marker = "libcgj-batch3480"
 *
 * Exclusive continuum CREATE-ONLY (3471-3480): stack_u64_init_u,
 * stack_u64_push_u, stack_u64_pop_u, stack_u64_peek_u, stack_u64_len_u,
 * queue_u64_init_u, queue_u64_push_u, queue_u64_pop_u, queue_u64_len_u,
 * batch_id_3480. Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* symbols — avoid multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3480_marker[] = "libcgj-batch3480";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b3480_id(void)
{
	return 3480u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_3480 - report this TU's graph batch number.
 *
 * Always returns 3480.
 */
uint32_t
gj_batch_id_3480(void)
{
	(void)NULL;
	return b3480_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_3480(void)
    __attribute__((alias("gj_batch_id_3480")));
