/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2620: milestone 2620 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2620(void);
 *     - Returns the compile-time graph batch number for this TU (2620).
 *   uint32_t gj_graph_milestone_2620(void);
 *     - Returns the current graph milestone revision (2620).
 *   uint32_t __gj_batch_id_2620  (alias)
 *   uint32_t __gj_graph_milestone_2620  (alias)
 *   __libcgj_batch2620_marker = "libcgj-batch2620"
 *
 * Milestone for the post-2610 mem exclusive wave (batches 2611-2619:
 * mem_zero_n_u, mem_fill_n_u, mem_copy_n_u, mem_move_n_u, mem_cmp_n_u,
 * mem_eq_n_u, mem_chr_n_u, mem_rchr_n_u, mem_len_until_u). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2620_marker[] = "libcgj-batch2620";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2620_id(void)
{
	return 2620u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2620 - report this TU's graph batch number.
 *
 * Always returns 2620.
 */
uint32_t
gj_batch_id_2620(void)
{
	(void)NULL;
	return b2620_id();
}

/*
 * gj_graph_milestone_2620 - report this TU's graph milestone revision.
 *
 * Always returns 2620 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2620(void)
{
	return b2620_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2620(void)
    __attribute__((alias("gj_batch_id_2620")));

uint32_t __gj_graph_milestone_2620(void)
    __attribute__((alias("gj_graph_milestone_2620")));
