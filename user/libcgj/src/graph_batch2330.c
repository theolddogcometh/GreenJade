/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2330: milestone 2330 batch identity (closes
 * post-2320 hash exclusive wave 2321-2330).
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2330(void);
 *     - Returns the compile-time graph batch number for this TU (2330).
 *   uint32_t gj_graph_milestone_2330(void);
 *     - Returns the current graph milestone revision (2330).
 *   uint32_t __gj_batch_id_2330  (alias)
 *   uint32_t __gj_graph_milestone_2330  (alias)
 *   __libcgj_batch2330_marker = "libcgj-batch2330"
 *
 * Milestone for the post-2320 hash exclusive wave (batches 2321-2329:
 * str_hash_djb2_u, str_hash_sdbm_u, str_hash_fnv1a32_u,
 * str_hash_fnv1a64_u, mem_hash_djb2_n, mem_hash_fnv1a32_n,
 * hash_mix_final32, hash_mix_final64, hash_seed_from_ptr). Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* or
 * milestone_* symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2330_marker[] = "libcgj-batch2330";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2330_id(void)
{
	return 2330u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2330 - report this TU's graph batch number.
 *
 * Always returns 2330.
 */
uint32_t
gj_batch_id_2330(void)
{
	(void)NULL;
	return b2330_id();
}

/*
 * gj_graph_milestone_2330 - report this TU's graph milestone revision.
 *
 * Always returns 2330 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2330(void)
{
	return b2330_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2330(void)
    __attribute__((alias("gj_batch_id_2330")));

uint32_t __gj_graph_milestone_2330(void)
    __attribute__((alias("gj_graph_milestone_2330")));
