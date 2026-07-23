/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2220: milestone 2220 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2220(void);
 *     - Returns the compile-time graph batch number for this TU (2220).
 *   uint32_t gj_graph_milestone_2220(void);
 *     - Returns the current graph milestone revision (2220).
 *   uint32_t __gj_batch_id_2220  (alias)
 *   uint32_t __gj_graph_milestone_2220  (alias)
 *   __libcgj_batch2220_marker = "libcgj-batch2220"
 *
 * Milestone for the post-2210 mem exclusive wave (batches 2211-2219:
 * mem_scan_eq_u32, mem_scan_eq_u64, mem_scan_neq_u8, mem_pref_eq_n,
 * mem_suff_eq_n, mem_overlap_p, mem_copy_fwd, mem_copy_bwd,
 * mem_fill_u8_n). Does NOT redefine gj_batch_id / gj_graph_milestone /
 * prior batch_id_* or milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2220_marker[] = "libcgj-batch2220";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2220_id(void)
{
	return 2220u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2220 - report this TU's graph batch number.
 *
 * Always returns 2220.
 */
uint32_t
gj_batch_id_2220(void)
{
	(void)NULL;
	return b2220_id();
}

/*
 * gj_graph_milestone_2220 - report this TU's graph milestone revision.
 *
 * Always returns 2220 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2220(void)
{
	return b2220_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2220(void)
    __attribute__((alias("gj_batch_id_2220")));

uint32_t __gj_graph_milestone_2220(void)
    __attribute__((alias("gj_graph_milestone_2220")));
