/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2750: milestone 2750 batch identity.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_2750(void);
 *     - Returns the compile-time graph batch number for this TU (2750).
 *   uint32_t gj_graph_milestone_2750(void);
 *     - Returns the current graph milestone revision (2750).
 *   uint32_t __gj_batch_id_2750  (alias)
 *   uint32_t __gj_graph_milestone_2750  (alias)
 *   __libcgj_batch2750_marker = "libcgj-batch2750"
 *
 * Milestone for the bitops exclusive wave (batches 2741-2749:
 * rotl_u32_u, rotr_u32_u, rotl_u64_u, rotr_u64_u, bswap16_bo_u,
 * bswap32_u, bswap64_u, memeq_n_u, mem_is_zero_n_u). Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* or milestone_*
 * symbols - avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2750_marker[] = "libcgj-batch2750";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b2750_id(void)
{
	return 2750u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_2750 - report this TU's graph batch number.
 *
 * Always returns 2750.
 */
uint32_t
gj_batch_id_2750(void)
{
	(void)NULL;
	return b2750_id();
}

/*
 * gj_graph_milestone_2750 - report this TU's graph milestone revision.
 *
 * Always returns 2750 (this milestone TU). Distinct from earlier
 * gj_graph_milestone / gj_graph_milestone_N so all can coexist at link time.
 */
uint32_t
gj_graph_milestone_2750(void)
{
	return b2750_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_batch_id_2750(void)
    __attribute__((alias("gj_batch_id_2750")));

uint32_t __gj_graph_milestone_2750(void)
    __attribute__((alias("gj_graph_milestone_2750")));
