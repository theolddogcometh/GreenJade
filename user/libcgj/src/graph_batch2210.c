/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2210: milestone 2210 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2210(void);
 *     - Returns the compile-time graph batch number for this TU (2210).
 *   uint32_t gj_graph_milestone_2210(void);
 *     - Returns the current graph milestone revision (2210).
 *   uint32_t __gj_batch_id_2210  (alias)
 *   uint32_t __gj_graph_milestone_2210  (alias)
 *   __libcgj_batch2210_marker = "libcgj-batch2210"
 *
 * Milestone for the post-2200 str exclusive wave (batches 2201-2209:
 * str_copy_n_u, str_cat_n_u, str_ncmp_u, str_nlen_u, str_chr_n_u,
 * str_rchr_n_u, str_spn_u, str_cspn_u, str_pbrk_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2210_marker[] = "libcgj-batch2210";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2210_id(void)
{
	return 2210u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2210 - report this TU's graph batch number.
 *
 * Always returns 2210.
 */
uint32_t
gj_batch_id_2210(void)
{
	(void)NULL;
	return b2210_id();
}

/*
 * gj_graph_milestone_2210 - report this TU's graph milestone revision.
 *
 * Always returns 2210 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2210(void)
{
	return b2210_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2210(void)
    __attribute__((alias("gj_batch_id_2210")));

uint32_t __gj_graph_milestone_2210(void)
    __attribute__((alias("gj_graph_milestone_2210")));
