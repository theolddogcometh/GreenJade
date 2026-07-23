/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2630: milestone 2630 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2630(void);
 *     - Returns the compile-time graph batch number for this TU (2630).
 *   uint32_t gj_graph_milestone_2630(void);
 *     - Returns the current graph milestone revision (2630).
 *   uint32_t __gj_batch_id_2630  (alias)
 *   uint32_t __gj_graph_milestone_2630  (alias)
 *   __libcgj_batch2630_marker = "libcgj-batch2630"
 *
 * Milestone for the post-2620 classic str exclusive wave (batches
 * 2621-2629: str_len_u, str_eq_u, str_cmp_u, str_cpy_u, str_cat_u,
 * str_chr_u, str_rchr_u, str_str_u, str_ncat_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2630_marker[] = "libcgj-batch2630";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2630_id(void)
{
	return 2630u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2630 - report this TU's graph batch number.
 *
 * Always returns 2630.
 */
uint32_t
gj_batch_id_2630(void)
{
	(void)NULL;
	return b2630_id();
}

/*
 * gj_graph_milestone_2630 - report this TU's graph milestone revision.
 *
 * Always returns 2630 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2630(void)
{
	return b2630_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2630(void)
    __attribute__((alias("gj_batch_id_2630")));

uint32_t __gj_graph_milestone_2630(void)
    __attribute__((alias("gj_graph_milestone_2630")));
